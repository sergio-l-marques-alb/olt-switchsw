/*
 * ! \file ctest_dnx_apt_object_mpls_def.c
 * Contains the functions and definitions for MPLS API performance test objects.
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

#include "ctest_dnx_apt.h"
#include <bcm/l3.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_MPLS_DEFAULT_NUMBER_OF_RUNS     10
#define DNX_APT_MPLS_DEFAULT_NUMBER_OF_CALLS    1000

/** EM rate */
#define DNX_APT_MPLS_INGRESS_INSERT_RATE        7000
#define DNX_APT_MPLS_EGRESS_INSERT_RATE         12000
#define DNX_APT_MPLS_INGRESS_PORT_INSERT_RATE   6000
#define DNX_APT_MPLS_EGRESS_PORT_INSERT_RATE    12500

#define DNX_APT_MPLS_INGRESS_INSERT_RATE_JR2_A0         6000
#define DNX_APT_MPLS_EGRESS_INSERT_RATE_JR2_A0          10000
#define DNX_APT_MPLS_INGRESS_PORT_INSERT_RATE_JR2_A0    5000
#define DNX_APT_MPLS_EGRESS_PORT_INSERT_RATE_JR2_A0     10000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_MPLS_TIME_THRESHOLD(_unit_, _test_, _test_type_) \
    dnxc_data_mgmt_is_jr2_a0(_unit_) ? \
            /** JR2_A0 */ DNX_APT_RATE_TO_TIME(DNX_APT_MPLS_##_test_##_##_test_type_##_RATE_JR2_A0) : \
            /** All other */ DNX_APT_RATE_TO_TIME(DNX_APT_MPLS_##_test_##_##_test_type_##_RATE)

#define DNX_APT_MPLS_OPTIONAL_THRESHOLD_INFO(_info_, _test_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "%.3fus (JR2_A0: %.3fus)", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_MPLS_##_test_##_##_test_type_##_RATE), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_MPLS_##_test_##_##_test_type_##_RATE_JR2_A0))

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for use with the MPLS API performance tests */
typedef struct
{
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_mpls_egress_label_t egress_label_array[3];
    int number_of_egress_labels;
    int arp_encap_id;
    bcm_mpls_port_t port;
    uint32 intf_insert;
    uint32 intf_update;
} dnx_apt_mpls_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/***********************************************************************************************************************
 * MPLS time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_mpls_ingress_insert_time_threshold(
    int unit)
{
    return DNX_APT_MPLS_TIME_THRESHOLD(unit, INGRESS, INSERT);
}
static double
dnx_apt_mpls_egress_insert_time_threshold(
    int unit)
{
    return DNX_APT_MPLS_TIME_THRESHOLD(unit, EGRESS, INSERT);
}
static double
dnx_apt_mpls_ingress_port_insert_time_threshold(
    int unit)
{
    return DNX_APT_MPLS_TIME_THRESHOLD(unit, INGRESS_PORT, INSERT);
}
static double
dnx_apt_mpls_egress_port_insert_time_threshold(
    int unit)
{
    return DNX_APT_MPLS_TIME_THRESHOLD(unit, EGRESS_PORT, INSERT);
}

/***********************************************************************************************************************
 * MPLS optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_mpls_ingress_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_MPLS_OPTIONAL_THRESHOLD_INFO(info, INGRESS, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_mpls_egress_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_MPLS_OPTIONAL_THRESHOLD_INFO(info, EGRESS, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_mpls_ingress_port_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_MPLS_OPTIONAL_THRESHOLD_INFO(info, INGRESS_PORT, INSERT);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_mpls_egress_port_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_MPLS_OPTIONAL_THRESHOLD_INFO(info, EGRESS_PORT, INSERT);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * MPLS shared init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_mpls_alloc(
    int unit,
    int number_of_entries,
    dnx_apt_mpls_info_t ** mpls_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*mpls_info, (sizeof(dnx_apt_mpls_info_t) * number_of_entries), "dnx_apt_mpls_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_mpls_generate(
    int unit,
    int number_of_entries,
    dnx_apt_mpls_info_t * mpls_info,
    uint8 is_egress,
    uint8 is_port,
    int number_of_egress_labels)
{
    int iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 1024 * 1024));

    dnx_apt_random_seed(0);
    for (iter = 0; iter < number_of_entries; iter++)
    {
        if (is_port)
        {
            bcm_gport_t gport_id;
            bcm_mpls_port_t_init(&mpls_info[iter].port);

            if (is_egress)
            {
                mpls_info[iter].port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
                mpls_info[iter].port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
                mpls_info[iter].port.encap_id = (dnx_apt_random_get() & 0x001FEFFE) + 0x1001;
                BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, mpls_info[iter].port.encap_id);
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_info[iter].port.mpls_port_id, gport_id);
                mpls_info[iter].port.egress_label.label = dnx_apt_unique_random_get(iter) & 0x000FFFFF;
                mpls_info[iter].port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
                mpls_info[iter].port.port = 201;
            }
            else
            {
                mpls_info[iter].port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
                mpls_info[iter].port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
                BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                           ((dnx_apt_random_get() & 0x001FEFFE) + 0x1001));
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_info[iter].port.mpls_port_id, gport_id);
                mpls_info[iter].port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
                mpls_info[iter].port.match_label = dnx_apt_unique_random_get(iter) & 0x000FFFFF;
                mpls_info[iter].port.port = 0;
            }
        }
        else
        {
            if (is_egress)
            {
                int label_iter;
                mpls_info[iter].number_of_egress_labels = number_of_egress_labels;

                for (label_iter = 0; label_iter < number_of_egress_labels; label_iter++)
                {
                    bcm_mpls_egress_label_t_init(&mpls_info[iter].egress_label_array[label_iter]);

                    mpls_info[iter].egress_label_array[label_iter].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID |
                        BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
                    BCM_L3_ITF_SET(mpls_info[iter].egress_label_array[label_iter].tunnel_id, BCM_L3_ITF_TYPE_LIF,
                                   ((dnx_apt_random_get() & 0x001FEFFE) + 0x1001));
                    mpls_info[iter].egress_label_array[label_iter].l3_intf_id = mpls_info[0].arp_encap_id;
                    mpls_info[iter].egress_label_array[label_iter].label = dnx_apt_unique_random_get(iter) & 0x000FFFFF;
                }
            }
            else
            {
                bcm_mpls_tunnel_switch_t_init(&mpls_info[iter].tunnel_switch);

                mpls_info[iter].tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
                mpls_info[iter].tunnel_switch.flags = BCM_MPLS_SWITCH_WITH_ID | BCM_MPLS_SWITCH_TTL_DECREMENT;
                mpls_info[iter].tunnel_switch.egress_if = 0;
                mpls_info[iter].tunnel_switch.tunnel_id = 0x4c000000 | ((dnx_apt_random_get() & 0x001FEFFE) + 0x1001);
                mpls_info[iter].tunnel_switch.vpn = 0;
                mpls_info[iter].tunnel_switch.label = dnx_apt_unique_random_get(iter) & 0x000FFFFF;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_apt_mpls_arp_create(
    int unit,
    int *arp_encap_id)
{
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_mac_t arp_mac_add = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };

    SHR_FUNC_INIT_VARS(unit);

    dnx_apt_random_seed(0);
    *arp_encap_id = 0x40000000 | (dnx_apt_random_get() & 0xFFFF);

    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, arp_mac_add, 6);
    l3eg.intf = 0;
    l3eg.encap_id = *arp_encap_id;
    l3eg.vlan = 123;
    l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, (BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY), &l3eg, &l3egid_null));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_apt_mpls_arp_destroy(
    int unit,
    uint32 arp_encap_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, arp_encap_id));
exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * MPLS test init/deinit functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_mpls_ingress_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_mpls_alloc(unit, number_of_calls, (dnx_apt_mpls_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_mpls_generate
                    (unit, number_of_calls, (dnx_apt_mpls_info_t *) * custom_data, FALSE, FALSE, 0));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_mpls_egress_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_mpls_alloc(unit, number_of_calls, (dnx_apt_mpls_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_mpls_arp_create(unit, &(((dnx_apt_mpls_info_t *) * custom_data)[0].arp_encap_id)));
    SHR_IF_ERR_EXIT(dnx_apt_mpls_generate
                    (unit, number_of_calls, (dnx_apt_mpls_info_t *) * custom_data, TRUE, FALSE, 1));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_mpls_egress_deinit(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_mpls_arp_destroy(unit, ((dnx_apt_mpls_info_t *) custom_data)[0].arp_encap_id));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_mpls_ingress_port_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_mpls_alloc(unit, number_of_calls, (dnx_apt_mpls_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_mpls_generate
                    (unit, number_of_calls, (dnx_apt_mpls_info_t *) * custom_data, FALSE, TRUE, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_mpls_egress_port_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_apt_mpls_alloc(unit, number_of_calls, (dnx_apt_mpls_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_mpls_generate(unit, number_of_calls, (dnx_apt_mpls_info_t *) * custom_data, TRUE, TRUE, 0));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * MPLS shared execution functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_mpls_ingress_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_mpls_tunnel_switch_create(unit, &((dnx_apt_mpls_info_t *) custom_data)[call_iter].tunnel_switch);
}

static shr_error_e
dnx_apt_mpls_egress_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_mpls_tunnel_initiator_create(unit, 0,
                                            ((dnx_apt_mpls_info_t *) custom_data)[call_iter].number_of_egress_labels,
                                            ((dnx_apt_mpls_info_t *) custom_data)[call_iter].egress_label_array);
}

static shr_error_e
dnx_apt_mpls_port_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_mpls_port_add(unit, 0, &((dnx_apt_mpls_info_t *) custom_data)[call_iter].port);
}

static shr_error_e
dnx_apt_mpls_ingress_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    return bcm_mpls_tunnel_switch_delete_all(unit);
}

static shr_error_e
dnx_apt_mpls_egress_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    return bcm_mpls_tunnel_initiator_clear_all(unit);
}

static shr_error_e
dnx_apt_mpls_port_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter;
    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, 0, ((dnx_apt_mpls_info_t *) custom_data)[iter].port.mpls_port_id));
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

/***********************************************************************************************************************
 * MPLS
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_mpls_ingress_insert_object = {
    /** Test name */
    "MPLS_Ingress_Insert",
    /** Test description */
    "Measures the performance of bcm_mpls_tunnel_switch_create() when inserting new random ingress labels.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_mpls_ingress_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_mpls_ingress_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_mpls_ingress_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_mpls_ingress_add_exec,
    /** Run deinit procedure */
    dnx_apt_mpls_ingress_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_mpls_egress_insert_object = {
    /** Test name */
    "MPLS_Egress_Insert",
    /** Test description */
    "Measures the performance of bcm_mpls_tunnel_initiator_create() when inserting new random egress labels.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_mpls_egress_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_mpls_egress_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_mpls_egress_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_mpls_egress_add_exec,
    /** Run deinit procedure */
    dnx_apt_mpls_egress_clear_exec,
    /** Test deinit procedure */
    dnx_apt_mpls_egress_deinit
};

const dnx_apt_object_def_t dnx_apt_mpls_ingress_port_insert_object = {
    /** Test name */
    "MPLS_Ingress_Port_Insert",
    /** Test description */
    "Measures the performance of bcm_mpls_port_add() when inserting new random MPLS ingress ports.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_mpls_ingress_port_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_mpls_ingress_port_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_mpls_ingress_port_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_mpls_port_add_exec,
    /** Run deinit procedure */
    dnx_apt_mpls_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_mpls_egress_port_insert_object = {
    /** Test name */
    "MPLS_Egress_Port_Insert",
    /** Test description */
    "Measures the performance of bcm_mpls_port_add() when inserting new random MPLS egress ports.",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_MPLS_DEFAULT_NUMBER_OF_CALLS,
    /** Time threshold */
    dnx_apt_mpls_egress_port_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_mpls_egress_port_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_mpls_egress_port_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_mpls_port_add_exec,
    /** Run deinit procedure */
    dnx_apt_mpls_port_clear_exec,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
