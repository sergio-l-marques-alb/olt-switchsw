/*
 * ! \file ctest_dnx_apt_object_oam_mep_def.c
 * Contains the functions and definitions for OAM MEP API performance test objects.
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

#include "ctest_dnx_apt.h"
#include <bcm/oam.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_RUNS     10
#define DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS    2000

#define DNX_APT_OAM_MEP_ETH_ACC_DNMEP_INSERT_RATE                 2800
#define DNX_APT_OAM_MEP_ETH_ACC_UPMEP_INSERT_RATE                 2500
#define DNX_APT_OAM_MEP_ETH_NON_ACC_DNMEP_INSERT_RATE            10000
#define DNX_APT_OAM_MEP_ETH_NON_ACC_UPMEP_INSERT_RATE            10000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_OAM_MEP_TIME_THRESHOLD(_unit_,  _test_obj_, _test_type_) \
        DNX_APT_RATE_TO_TIME(DNX_APT_OAM_MEP_##_test_obj_##_##_test_type_##_RATE)

#define DNX_APT_OAM_MEP_OPTIONAL_THRESHOLD_INFO(_info_, _test_obj_, _test_type_) \
            sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus", \
                        DNX_APT_RATE_TO_TIME(DNX_APT_OAM_MEP_##_test_obj_##_##_test_type_##_RATE))

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the OAM MEP API performance tests */
typedef struct
{
    bcm_oam_endpoint_info_t ep;
} dnx_apt_oam_mep_info_t;

typedef enum dnx_apt_oam_mep_test_obj_e
{
    dnxAPTOAMMepTestEthDnmepAcc = 0,
    dnxAPTOAMMepTestEthDnmepNonAcc = 1,
    dnxAPTOAMMepTestEthUpmepAcc = 2,
    dnxAPTOAMMepTestEthUpmepNonAcc = 3,
    dnxAPTOAMMepTestCount = 4   /* Always Last. Not a usable value. */
} dnx_apt_oam_mep_test_obj_t;

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

static bcm_gport_t ctest_oam_mep_gports[DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS];
bcm_oam_group_info_t group_info_long;
bcm_oam_profile_t ingress_profile = 1;
bcm_oam_profile_t egress_profile = 1;
bcm_oam_profile_t acc_ingress_profile = 1;
bcm_oam_profile_t acc_egress_profile = 1;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/***********************************************************************************************************************
 * OAM MEP time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_oam_mep_eth_acc_dnmep_insert_time_threshold(
    int unit)
{
    return DNX_APT_OAM_MEP_TIME_THRESHOLD(unit, ETH_ACC_DNMEP, INSERT);
}

/***********************************************************************************************************************
 * OAM MEP time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_oam_mep_eth_acc_upmep_insert_time_threshold(
    int unit)
{
    return DNX_APT_OAM_MEP_TIME_THRESHOLD(unit, ETH_ACC_UPMEP, INSERT);
}

/***********************************************************************************************************************
 * OAM MEP time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_oam_mep_eth_non_acc_dnmep_insert_time_threshold(
    int unit)
{
    return DNX_APT_OAM_MEP_TIME_THRESHOLD(unit, ETH_NON_ACC_DNMEP, INSERT);
}

/***********************************************************************************************************************
 * OAM MEP time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_oam_mep_eth_non_acc_upmep_insert_time_threshold(
    int unit)
{
    return DNX_APT_OAM_MEP_TIME_THRESHOLD(unit, ETH_NON_ACC_UPMEP, INSERT);
}

/***********************************************************************************************************************
 * OAM MEP optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_acc_dnmep_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_OAM_MEP_OPTIONAL_THRESHOLD_INFO(info, ETH_ACC_DNMEP, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * OAM MEP optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_acc_upmep_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_OAM_MEP_OPTIONAL_THRESHOLD_INFO(info, ETH_ACC_UPMEP, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * OAM MEP optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_non_acc_dnmep_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_OAM_MEP_OPTIONAL_THRESHOLD_INFO(info, ETH_NON_ACC_DNMEP, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * OAM MEP optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_non_acc_upmep_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_OAM_MEP_OPTIONAL_THRESHOLD_INFO(info, ETH_NON_ACC_UPMEP, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * AC LIF Create functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_lif_create(
    int unit,
    int number_of_meps)
{
    int iter = 0;
    bcm_vlan_port_t port_info;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_meps; iter++)
    {
        bcm_vlan_port_t_init(&port_info);
        port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        port_info.port = 200;
        port_info.match_vlan = iter + 1;
        SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &port_info));

        ctest_oam_mep_gports[iter] = port_info.vlan_port_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM Group Create functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_group_create(
    int unit)
{
    uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH] = { 1, 32, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd };

    SHR_FUNC_INIT_VARS(unit);

    bcm_oam_group_info_t_init(&group_info_long);
    sal_memcpy(group_info_long.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);

    SHR_IF_ERR_EXIT(bcm_oam_group_create(unit, &group_info_long));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM Group Create functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_lif_profile_config(
    int unit,
    int number_of_meps)
{
    uint32 flags = 0;
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_oam_profile_id_get_by_type
                    (unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile));
    SHR_IF_ERR_EXIT(bcm_oam_profile_id_get_by_type
                    (unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile));
    SHR_IF_ERR_EXIT(bcm_oam_profile_id_get_by_type
                    (unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile));
    SHR_IF_ERR_EXIT(bcm_oam_profile_id_get_by_type
                    (unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile));

    for (iter = 0; iter < number_of_meps; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_oam_lif_profile_set(unit, 0, ctest_oam_mep_gports[iter], ingress_profile, egress_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_alloc(
    int unit,
    int number_of_meps,
    dnx_apt_oam_mep_info_t ** oam_mep_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*oam_mep_info, (sizeof(dnx_apt_oam_mep_info_t) * number_of_meps), "dnx_apt_oam_mep_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_oam_mep_generate(
    int unit,
    dnx_apt_oam_mep_test_obj_t test_obj,
    int number_of_meps,
    dnx_apt_oam_mep_info_t * oam_mep_info)
{
    int iter = 0;
    bcm_oam_endpoint_info_t *pEndpoint;
    bcm_mac_t src_mac_add = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
    int flags_stats_supported[] = {
        0,
        BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS,
        BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS,
        BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS | BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS,
        BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS | BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS,
        BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS | BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS,
        BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS | BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS |
            BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS,
    };
    int ccm_period_supported[] = { 0,
        BCM_OAM_ENDPOINT_CCM_PERIOD_3MS,
        BCM_OAM_ENDPOINT_CCM_PERIOD_10MS,
        BCM_OAM_ENDPOINT_CCM_PERIOD_100MS,
        BCM_OAM_ENDPOINT_CCM_PERIOD_1S,
        BCM_OAM_ENDPOINT_CCM_PERIOD_10S,
        BCM_OAM_ENDPOINT_CCM_PERIOD_1M,
        BCM_OAM_ENDPOINT_CCM_PERIOD_10M
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS));
    dnx_apt_random_seed(0);

    SHR_NULL_CHECK(oam_mep_info, _SHR_E_INTERNAL, "oam_mep_info");
    for (iter = 0; iter < number_of_meps; iter++)
    {
        pEndpoint = &oam_mep_info[iter].ep;
        bcm_oam_endpoint_info_t_init(pEndpoint);

        pEndpoint->endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
        pEndpoint->type = bcmOAMEndpointTypeEthernet;
        pEndpoint->group = group_info_long.id;

        pEndpoint->gport =
            ctest_oam_mep_gports[dnx_apt_unique_random_get(iter) % DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS];
        pEndpoint->level = dnx_apt_random_get() & 0x7;
        if (test_obj == dnxAPTOAMMepTestEthDnmepAcc || test_obj == dnxAPTOAMMepTestEthUpmepAcc)
        {
            if (test_obj == dnxAPTOAMMepTestEthDnmepAcc)
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(pEndpoint->tx_gport, 200);
                pEndpoint->acc_profile_id = acc_ingress_profile;
            }
            else if (test_obj == dnxAPTOAMMepTestEthUpmepAcc)
            {
                pEndpoint->flags |= BCM_OAM_ENDPOINT_UP_FACING;
                pEndpoint->tx_gport = BCM_GPORT_INVALID;
                pEndpoint->acc_profile_id = acc_egress_profile;
            }
            pEndpoint->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
            pEndpoint->name = 456;
            pEndpoint->outer_tpid = 0x8100;
            pEndpoint->inner_tpid = 0x9100;
            pEndpoint->int_pri = 0;

            pEndpoint->ccm_period = ccm_period_supported[dnx_apt_random_get() & 0x7];
            pEndpoint->vlan = dnx_apt_random_get() % 0xFFE;
            pEndpoint->inner_vlan = dnx_apt_random_get() % 0xFFE;
            pEndpoint->pkt_pri = dnx_apt_random_get() & 0xF;
            pEndpoint->inner_pkt_pri = dnx_apt_random_get() & 0xF;
            src_mac_add[5] = dnx_apt_random_get() & 0xFF;
            sal_memcpy(pEndpoint->src_mac_address, src_mac_add, 6);
            pEndpoint->flags2 |= flags_stats_supported[dnx_apt_random_get() % 7];
        }
        else
        {
            if (test_obj == dnxAPTOAMMepTestEthUpmepNonAcc)
            {
                pEndpoint->flags |= BCM_OAM_ENDPOINT_UP_FACING;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_acc_dnmep_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_create(unit, number_of_calls));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_group_create(unit));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_profile_config(unit, number_of_calls));

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_alloc(unit, number_of_calls, (dnx_apt_oam_mep_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_generate
                    (unit, dnxAPTOAMMepTestEthDnmepAcc, number_of_calls, (dnx_apt_oam_mep_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_acc_upmep_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_create(unit, number_of_calls));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_group_create(unit));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_profile_config(unit, number_of_calls));

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_alloc(unit, number_of_calls, (dnx_apt_oam_mep_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_generate
                    (unit, dnxAPTOAMMepTestEthUpmepAcc, number_of_calls, (dnx_apt_oam_mep_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_non_acc_dnmep_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_create(unit, number_of_calls));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_group_create(unit));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_profile_config(unit, number_of_calls));

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_alloc(unit, number_of_calls, (dnx_apt_oam_mep_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_generate
                    (unit, dnxAPTOAMMepTestEthDnmepNonAcc, number_of_calls, (dnx_apt_oam_mep_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_eth_non_acc_upmep_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_create(unit, number_of_calls));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_group_create(unit));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_lif_profile_config(unit, number_of_calls));

    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_alloc(unit, number_of_calls, (dnx_apt_oam_mep_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_oam_mep_generate
                    (unit, dnxAPTOAMMepTestEthUpmepNonAcc, number_of_calls, (dnx_apt_oam_mep_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_oam_endpoint_create(unit, &((dnx_apt_oam_mep_info_t *) custom_data)[call_iter].ep));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_oam_mep_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_oam_endpoint_destroy(unit, ((dnx_apt_oam_mep_info_t *) custom_data)[iter].ep.id));
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * OAM MEP test deinit functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_oam_mep_deinit(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_oam_group_destroy(unit, group_info_long.id));

    for (iter = 0; iter < number_of_calls; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, ctest_oam_mep_gports[iter]));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * GLOBALs
 * {
 */

const dnx_apt_object_def_t dnx_apt_oam_mep_eth_acc_dnmep_insert_object = {
    /** Test name */
    "OAM_MEP_ETH_ACC_DOWN_Insert",
    /** Test description */
    "Measures the performance of bcm_oam_endpoint_create() when inserting acc eth downmep entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_oam_mep_eth_acc_dnmep_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_oam_mep_eth_acc_dnmep_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_oam_mep_eth_acc_dnmep_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_oam_mep_add_exec,
    /** Run deinit procedure */
    dnx_apt_oam_mep_clear_exec,
    /** Test deinit procedure */
    dnx_apt_oam_mep_deinit
};

const dnx_apt_object_def_t dnx_apt_oam_mep_eth_acc_upmep_insert_object = {
    /** Test name */
    "OAM_MEP_ETH_ACC_UP_Insert",
    /** Test description */
    "Measures the performance of bcm_oam_endpoint_create() when inserting acc eth upmep entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_oam_mep_eth_acc_upmep_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_oam_mep_eth_acc_upmep_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_oam_mep_eth_acc_upmep_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_oam_mep_add_exec,
    /** Run deinit procedure */
    dnx_apt_oam_mep_clear_exec,
    /** Test deinit procedure */
    dnx_apt_oam_mep_deinit
};

const dnx_apt_object_def_t dnx_apt_oam_mep_eth_non_acc_dnmep_insert_object = {
    /** Test name */
    "OAM_MEP_ETH_NON_ACC_DOWN_Insert",
    /** Test description */
    "Measures the performance of bcm_oam_endpoint_create() when inserting non-acc eth downmep entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_oam_mep_eth_non_acc_dnmep_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_oam_mep_eth_non_acc_dnmep_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_oam_mep_eth_non_acc_dnmep_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_oam_mep_add_exec,
    /** Run deinit procedure */
    dnx_apt_oam_mep_clear_exec,
    /** Test deinit procedure */
    dnx_apt_oam_mep_deinit
};

const dnx_apt_object_def_t dnx_apt_oam_mep_eth_non_acc_upmep_insert_object = {
    /** Test name */
    "OAM_MEP_ETH_NON_ACC_UP_Insert",
    /** Test description */
    "Measures the performance of bcm_oam_endpoint_create() when inserting non acc eth upmep entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_OAM_MEP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_oam_mep_eth_non_acc_upmep_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_oam_mep_eth_non_acc_upmep_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_oam_mep_eth_non_acc_upmep_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_oam_mep_add_exec,
    /** Run deinit procedure */
    dnx_apt_oam_mep_clear_exec,
    /** Test deinit procedure */
    dnx_apt_oam_mep_deinit
};

/*
 * }
 */
