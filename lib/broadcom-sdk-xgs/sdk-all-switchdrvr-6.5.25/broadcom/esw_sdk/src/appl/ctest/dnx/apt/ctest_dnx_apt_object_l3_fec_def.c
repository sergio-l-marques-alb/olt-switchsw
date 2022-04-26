/*
* ! \file ctest_dnx_apt_object_l3_fec_def.c
* Contains the functions and definitions for L3 FEC API performance test objects.
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
#include <bcm/l3.h>
#include <bcm/switch.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/mdb_internal_shared.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_RUNS      10
#define DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_CALLS     10000

#define DNX_APT_L3_FEC_INSERT_RATE_NO_HIT  36000
#define DNX_APT_L3_FEC_UPDATE_RATE_NO_HIT  31000
#define DNX_APT_L3_FEC_GET_RATE_NO_HIT  42000
#define DNX_APT_L3_FEC_DESTROY_RATE_NO_HIT 65000
#define DNX_APT_L3_FEC_INSERT_RATE_HIT     61000
#define DNX_APT_L3_FEC_UPDATE_RATE_HIT     20000
#define DNX_APT_L3_FEC_GET_RATE_HIT     42000
#define DNX_APT_L3_FEC_DESTROY_RATE_HIT    75000

/*
 * }
 */

/*
 * MACROs
 * {
 */

/** Utility macros for unifying similar way of handling time threshold and optional threshold information */
#define DNX_APT_L3_FEC_TIME_THRESHOLD(_unit_, _hit_bit_accessible_, _test_type_) \
        _hit_bit_accessible_ ? \
                /** HIT bit not supported */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_FEC_##_test_type_##_RATE_NO_HIT) : \
                /** HIT bit supported */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_FEC_##_test_type_##_RATE_HIT)

#define DNX_APT_L3_FEC_OPTIONAL_THRESHOLD_INFO(_info_, _test_type_) \
    sal_snprintf(_info_, DNX_APT_MAX_DESCRIPTION_LENGTH - 1, "HIT: %.3fus \nNO_HIT: %.3fus", \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_FEC_##_test_type_##_RATE_HIT), \
                 DNX_APT_RATE_TO_TIME(DNX_APT_L3_FEC_##_test_type_##_RATE_NO_HIT))
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Utility structure for used with the L3 FEC API performance tests */
typedef struct
{
    bcm_l3_egress_t fec_struct;
    int fec_id;
    uint32 arp_update;
    uint32 port_update;
    uint32 flags;
} dnx_apt_l3_fec_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/***********************************************************************************************************************
 * L3 FEC time threshold functions
 **********************************************************************************************************************/
static double
dnx_apt_l3_fec_insert_time_threshold(
    int unit)
{
    uint8 fec_hit = dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit);
    return DNX_APT_L3_FEC_TIME_THRESHOLD(unit, fec_hit, INSERT);
}

static double
dnx_apt_l3_fec_update_time_threshold(
    int unit)
{
    uint8 fec_hit = dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit);
    return DNX_APT_L3_FEC_TIME_THRESHOLD(unit, fec_hit, UPDATE);
}
static double
dnx_apt_l3_fec_get_time_threshold(
    int unit)
{
    uint8 fec_hit = dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit);
    return DNX_APT_L3_FEC_TIME_THRESHOLD(unit, fec_hit, GET);
}
static double
dnx_apt_l3_fec_destroy_time_threshold(
    int unit)
{
    uint8 fec_hit = dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit);
    return DNX_APT_L3_FEC_TIME_THRESHOLD(unit, fec_hit, DESTROY);
}

/***********************************************************************************************************************
 * L3 FEC optional threshold info functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_fec_insert_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_FEC_OPTIONAL_THRESHOLD_INFO(info, INSERT);
    return _SHR_E_NONE;
}

static shr_error_e
dnx_apt_l3_fec_update_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_FEC_OPTIONAL_THRESHOLD_INFO(info, UPDATE);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_l3_fec_get_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_FEC_OPTIONAL_THRESHOLD_INFO(info, GET);
    return _SHR_E_NONE;
}
static shr_error_e
dnx_apt_l3_fec_destroy_optional_threshold_info(
    int unit,
    char info[DNX_APT_MAX_DESCRIPTION_LENGTH])
{
    DNX_APT_L3_FEC_OPTIONAL_THRESHOLD_INFO(info, DESTROY);
    return _SHR_E_NONE;
}

/***********************************************************************************************************************
 * L3 FEC shared init functions
 **********************************************************************************************************************/
/** A function to generate a non-consecutive unique sequence of given number of FEC IDs */
shr_error_e
dnx_l3_performance_fec_ids_generate(
    int unit,
    int *fec_ids,
    uint32 nof_entries,
    int *hierarchy)
{
    int id;
    int hier = dnx_apt_random_get() % dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);
    uint32 base_fec;
    uint32 fec_range_size;
    int fec_count = 0;
    int hierarchy_count = 0;
    mdb_physical_table_e fec_db;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fec_db.get(unit, DNX_L3_FEC_DIRECTION_NA, hier, &fec_db));
    SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &base_fec, &fec_range_size));
    for (id = 0; id < nof_entries; id++)
    {
        while (base_fec + fec_count == base_fec + fec_range_size)
        {
            if (hierarchy_count == dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "All the hierarchies FECs are less than the number of entries\n");
            }
            hier = (hier + 1) % dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit);
            SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fec_db.get(unit, DNX_L3_FEC_DIRECTION_NA, hier, &fec_db));
            SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &base_fec, &fec_range_size));
            fec_count = 0;
            hierarchy_count++;
        }

        fec_ids[id] = base_fec + fec_count;
        hierarchy[id] = hier;
        fec_count++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_fec_alloc(
    int unit,
    int number_of_fecs,
    dnx_apt_l3_fec_info_t ** l3_fec_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l3_fec_info, (sizeof(dnx_apt_l3_fec_info_t) * number_of_fecs), "dnx_apt_l3_fec_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_fec_generate(
    int unit,
    int number_of_fecs,
    dnx_apt_l3_fec_info_t * l3_fec_info)
{
    int iter = 0;
    int *fec_ids;
    int *hierarchy;
    uint32 fwd_flags[3] = { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };
    bcm_port_t port = 200;
    bcm_port_t port_updated = 201;
    bcm_gport_t gport = 0;
    bcm_gport_t gport_updated = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_unique_random_generate(unit, 16 * 1024 * 1024));

    dnx_apt_random_seed(0);
    hierarchy = (int *) sal_alloc(sizeof(int) * number_of_fecs, "hierarchy int");
    fec_ids = (int *) sal_alloc(sizeof(int) * number_of_fecs, "fec_ids int");
    SHR_IF_ERR_EXIT(dnx_l3_performance_fec_ids_generate(unit, fec_ids, number_of_fecs, hierarchy));
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport_updated, port_updated);

    for (iter = 0; iter < number_of_fecs; iter++)
    {
        bcm_l3_egress_t_init(&l3_fec_info[iter].fec_struct);
        l3_fec_info[iter].fec_struct.flags |= fwd_flags[hierarchy[iter]];
        l3_fec_info[iter].fec_struct.flags2 = BCM_L3_FLAGS2_SKIP_HIT_CLEAR;
        l3_fec_info[iter].fec_struct.intf = (iter & 0xFF0) + 1;
        l3_fec_info[iter].fec_struct.port = gport;
        l3_fec_info[iter].fec_struct.encap_id = 0x1000 + iter;
        l3_fec_info[iter].arp_update = 0x1000 + iter + 1;
        l3_fec_info[iter].port_update = gport_updated;
        l3_fec_info[iter].fec_id = fec_ids[iter];
        l3_fec_info[iter].flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    }

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 fec test init functions
 **********************************************************************************************************************/
static shr_error_e
dnx_apt_l3_fec_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    SHR_FUNC_INIT_VARS(unit);
    key.type = bcmSwitchModuleVerifyEnable;
    key.index = bcmModuleL3EgressFec;
    value.value = 0;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value));
    key.type = bcmSwitchModuleErrorRecoveryEnable;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value));

    SHR_IF_ERR_EXIT(dnx_apt_l3_fec_alloc(unit, number_of_calls, (dnx_apt_l3_fec_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_fec_generate(unit, number_of_calls, (dnx_apt_l3_fec_info_t *) * custom_data));

exit:
    SHR_FUNC_EXIT;
}

/***********************************************************************************************************************
 * L3 FEC shared execution functions
 **********************************************************************************************************************/

static shr_error_e
dnx_apt_l3_fec_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_l3_egress_create(unit, ((dnx_apt_l3_fec_info_t *) custom_data)[call_iter].flags,
                                &((dnx_apt_l3_fec_info_t *) custom_data)[call_iter].fec_struct,
                                &((dnx_apt_l3_fec_info_t *) custom_data)[call_iter].fec_id);
}

/**
* \brief
*  L3 FEC get exec function, called in the execute stage of get APT test.
*  This function calls to bcm_l3_egress_get of the FEC in the current call_iter.
* \param [in] unit - The unit number.
* \param [in] call_iter - The current call_iter of the APT test.
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_fec_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
dnx_apt_l3_fec_get_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    bcm_l3_egress_t egress_object;
    dnx_apt_l3_fec_info_t *l3_fec_info = NULL;
    SHR_FUNC_INIT_VARS(unit);
    l3_fec_info = &((dnx_apt_l3_fec_info_t *) custom_data)[call_iter];
    SHR_IF_ERR_EXIT(bcm_l3_egress_get(unit, l3_fec_info->fec_id, &egress_object));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_apt_l3_fec_destroy_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    return bcm_l3_egress_destroy(unit, ((dnx_apt_l3_fec_info_t *) custom_data)[call_iter].fec_id);
}

static shr_error_e
dnx_apt_l3_fec_update_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int iter = 0;
    dnx_apt_l3_fec_info_t *l3_fec_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < number_of_calls; iter++)
    {
        l3_fec_info = &((dnx_apt_l3_fec_info_t *) custom_data)[iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY, &l3_fec_info->fec_struct, &l3_fec_info->fec_id));
        l3_fec_info->fec_struct.encap_id = l3_fec_info->arp_update;
        l3_fec_info->fec_struct.port = l3_fec_info->port_update;
        l3_fec_info->flags |= BCM_L3_REPLACE;
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_l3_egress_destroy_cb(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * info,
    void *user_data)
{
    return bcm_l3_egress_destroy(unit, intf);;
}

static shr_error_e
dnx_apt_l3_fec_clear_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    SHR_FUNC_INIT_VARS(unit);
    key.type = bcmSwitchModuleVerifyEnable;
    key.index = bcmModuleL3EgressFec;
    value.value = 1;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value));
    key.type = bcmSwitchModuleErrorRecoveryEnable;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value));
    SHR_IF_ERR_EXIT(bcm_l3_egress_traverse(unit, dnx_l3_egress_destroy_cb, NULL));
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
 * IPv4
 **********************************************************************************************************************/
const dnx_apt_object_def_t dnx_apt_l3_fec_insert_object = {
    /** Test name */
    "L3_FEC_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_egress_create() when inserting new non-consecutive FEC entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC,
    /** Time threshold */
    dnx_apt_l3_fec_insert_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_fec_insert_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_fec_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_fec_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_fec_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_fec_update_object = {
    /** Test name */
    "L3_FEC_Update",
    /** Test description */
    "Measures the performance of bcm_l3_egress_create() when updating existing non-consecutive FEC entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC,
    /** Time threshold */
    dnx_apt_l3_fec_update_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_fec_update_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_fec_init,
    /** Run init procedure */
    dnx_apt_l3_fec_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_fec_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_fec_clear_exec,
    /** Test deinit procedure */
    NULL
};
const dnx_apt_object_def_t dnx_apt_l3_fec_get_object = {
    /** Test name */
    "L3_FEC_Get",
    /** Test description */
    "Measures the performance of bcm_l3_egress_get() when performing get on existing FEC entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC,
    /** Time threshold */
    dnx_apt_l3_fec_get_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_fec_get_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_fec_init,
    /** Run init procedure */
    dnx_apt_l3_fec_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_fec_get_exec,
    /** Run deinit procedure */
    dnx_apt_l3_fec_clear_exec,
    /** Test deinit procedure */
    NULL
};

const dnx_apt_object_def_t dnx_apt_l3_fec_destroy_object = {
    /** Test name */
    "L3_FEC_Destroy",
    /** Test description */
    "Measures the performance of bcm_l3_egress_destroy() when deleting existing non-consecutive FEC entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_FEC_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC,
    /** Time threshold */
    dnx_apt_l3_fec_destroy_time_threshold,
    /** Optional threshold info */
    dnx_apt_l3_fec_destroy_optional_threshold_info,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_fec_init,
    /** Run init procedure */
    dnx_apt_l3_fec_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_fec_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    NULL
};

/*
 * }
 */
