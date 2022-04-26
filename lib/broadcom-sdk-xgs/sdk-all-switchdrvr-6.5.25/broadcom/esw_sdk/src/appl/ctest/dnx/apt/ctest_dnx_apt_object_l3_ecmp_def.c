/* * ! \file ctest_dnx_apt_object_l3_ecmp_def.c
 * Contains the functions and definitions for L3 ECMP API performance test objects.
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

/*
* APT L3 ECMP TESTS DEFINES
*/
#define DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS      10
#define DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS     38000

/*
* APT L3 ECMP TESTS MAX RATE ALLOWED
*/
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_INSERT_RATE_BIG_GROUP  9000
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_UPDATE_RATE_BIG_GROUP  8800
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_GET_RATE_BIG_GROUP  42000
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_DESTROY_RATE_BIG_GROUP 35000

#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_INSERT_RATE_SMALL_GROUP  9400
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_UPDATE_RATE_SMALL_GROUP 9400
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_GET_RATE_SMALL_GROUP 40000
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_DESTROY_RATE_SMALL_GROUP 41000

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_INSERT_RATE_BIG_GROUP  28000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_UPDATE_RATE_BIG_GROUP  29000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_INSERT_RATE_SMALL_GROUP  30000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_UPDATE_RATE_SMALL_GROUP 30000

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_INSERT_RATE_BIG_GROUP  5100
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_UPDATE_RATE_BIG_GROUP  5000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_INSERT_RATE_SMALL_GROUP  5000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_UPDATE_RATE_SMALL_GROUP 5000

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_INSERT_RATE_BIG_GROUP  5000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_UPDATE_RATE_BIG_GROUP  5000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_INSERT_RATE_SMALL_GROUP 5000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_UPDATE_RATE_SMALL_GROUP 5100

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_DESTROY_RATE_SMALL_GROUP 38000
#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_DESTROY_RATE_BIG_GROUP 33000

/** MAX NOF FECs in tDNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGEhe ECMP group */
#define NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST 256

/*
 * MACROs
 * {
 */

/*
 * Utility macros for unifying similar way of handling time threshold information,
 * for working with max size of ECMP group there are different rate values
 * */
#define DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_TIME_THRESHOLD(_unit_ , _test_type_) \
                (dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit) == DNX_DATA_MAX_L3_ECMP_GROUP_SIZE_MULTIPLY_AND_DIVIDE_NOF_BITS) ? \
                /** working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_##_test_type_##_RATE_BIG_GROUP) : \
                /** not working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_##_test_type_##_RATE_SMALL_GROUP)

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_TIME_THRESHOLD(_unit_ , _test_type_) \
                (dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit) == DNX_DATA_MAX_L3_ECMP_GROUP_SIZE_MULTIPLY_AND_DIVIDE_NOF_BITS) ? \
                /** working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_##_test_type_##_RATE_BIG_GROUP) : \
                /** not working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_##_test_type_##_RATE_SMALL_GROUP)

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_TIME_THRESHOLD(_unit_ , _test_type_) \
                (dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit) == DNX_DATA_MAX_L3_ECMP_GROUP_SIZE_MULTIPLY_AND_DIVIDE_NOF_BITS) ? \
                /** working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_##_test_type_##_RATE_BIG_GROUP) : \
                /** not working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_##_test_type_##_RATE_SMALL_GROUP)

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_TIME_THRESHOLD(_unit_ , _test_type_) \
                (dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit) == DNX_DATA_MAX_L3_ECMP_GROUP_SIZE_MULTIPLY_AND_DIVIDE_NOF_BITS) ? \
                /** working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_##_test_type_##_RATE_BIG_GROUP) : \
                /** not working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_##_test_type_##_RATE_SMALL_GROUP)

#define DNX_APT_L3_ECMP_CONSISTENT_HASHING_DESTROY_TIME_THRESHOLD(_unit_) \
                (dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit) == DNX_DATA_MAX_L3_ECMP_GROUP_SIZE_MULTIPLY_AND_DIVIDE_NOF_BITS) ? \
                /** working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_DESTROY_RATE_BIG_GROUP) : \
                /** not working with max size of ECMP group */ DNX_APT_RATE_TO_TIME(DNX_APT_L3_ECMP_CONSISTENT_HASHING_DESTROY_RATE_SMALL_GROUP)

/*
 * TYPEDEFs
 * {
 */

/*
 * \brief The following enumeration represent different test cases of the consistent tables.
 * each enumeration describes different table size
 */
typedef enum
{
    /*
     * Test case were the ecmp's table size is small
     */
    APT_ECMP_CONSISTENT_HASING_WITH_SMALL_TABLES,
    /*
     * Test case were the ecmp's table size is medium
     */
    APT_ECMP_CONSISTENT_HASING_WITH_MEDIUM_TABLES,
    /*
     * Test case were the ecmp's table size is Large
     */
    APT_ECMP_CONSISTENT_HASING_WITH_LARGE_TABLES
} apt_ecmp_consistent_test_table_size_type_e;

/** Utility structure used with the L3 ECMP API performance tests for saving ECMP data*/
typedef struct
{
    bcm_l3_egress_ecmp_t ecmp_struct;
    bcm_if_t *fec_ids;
    int count_intf;
    int ecmp_id;
    int nof_members;
    int ecmp_update;
    uint32 flags;
} dnx_apt_l3_ecmp_info_t;

/** Utility structure for used with the L3 ECMP API performance tests for saving FEC data*/
typedef struct
{
    bcm_l3_egress_t fec_struct;
    int fec_id;
    uint32 flags;
} dnx_apt_l3_fec_info_t;

static bcm_if_t *intf_array_by_hier[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES];
dnx_apt_l3_fec_info_t *l3_fec_info[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES];
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
 * APT L3 ECMP time threshold functions, for each test type in every mode
 */

static double
dnx_apt_l3_ecmp_insert_multiply_and_divide_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_TIME_THRESHOLD(unit, INSERT);
}

static double
dnx_apt_l3_ecmp_update_multiply_and_divide_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_TIME_THRESHOLD(unit, UPDATE);
}
static double
dnx_apt_l3_ecmp_get_multiply_and_divide_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_TIME_THRESHOLD(unit, GET);
}
static double
dnx_apt_l3_ecmp_destroy_multiply_and_divide_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_MULTIPLY_AND_DIVIDE_TIME_THRESHOLD(unit, DESTROY);
}
static double
dnx_apt_l3_ecmp_insert_consistent_hashing_small_table_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_TIME_THRESHOLD(unit, INSERT);
}
static double
dnx_apt_l3_ecmp_insert_consistent_hashing_medium_table_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_TIME_THRESHOLD(unit, INSERT);
}
static double
dnx_apt_l3_ecmp_insert_consistent_hashing_large_table_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_TIME_THRESHOLD(unit, INSERT);
}

static double
dnx_apt_l3_ecmp_update_consistent_hashing_small_table_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_SMALL_TABLE_TIME_THRESHOLD(unit, UPDATE);
}
static double
dnx_apt_l3_ecmp_update_consistent_hashing_medium_table_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_MEDIUM_TABLE_TIME_THRESHOLD(unit, UPDATE);

}
static double
dnx_apt_l3_ecmp_update_consistent_hashing_large_table_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_LARGE_TABLE_TIME_THRESHOLD(unit, UPDATE);

}
static double
dnx_apt_l3_ecmp_destroy_consistent_hashing_time_threshold(
    int unit)
{
    return DNX_APT_L3_ECMP_CONSISTENT_HASHING_DESTROY_TIME_THRESHOLD(unit);

}

/*
 * Externs
 * {
 */
extern int get_fec_range(
    int unit,
    int hierarchy,
    int *start,
    int *nof_fecs);
/*
 * }
 */

/**
 * L3 ECMP create multiply and divide functions
*/

/**
* \brief
*  L3 ECMP allocation function - This function allocates memory space for all of the ECMPS structures
 * and for all FEC members structures as well.
* \param [in] unit - The unit number.
* \param [in] number_of_ecmps - The number of ECMPs in the test that needed to be allocated.
* \param [in]- **l3_ecmp_info - a pointer to ECMPs object array - this array has number_of_ecmps entries, each one for
*                               a ECMP structure
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_multiply_and_divide_mem_alloc(
    int unit,
    int number_of_ecmps,
    dnx_apt_l3_ecmp_info_t ** l3_ecmp_info)
{
    int hier_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l3_ecmp_info, (sizeof(dnx_apt_l3_ecmp_info_t) * number_of_ecmps), "dnx_apt_l3_ecmp_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (hier_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hier_iter++)
    {
        SHR_ALLOC(intf_array_by_hier[hier_iter], (sizeof(bcm_if_t) * NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST),
                  "dnx_apt_l3_ecmp_hier", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC(l3_fec_info[hier_iter], (sizeof(dnx_apt_l3_fec_info_t) * NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST),
                  "dnx_apt_l3_ecmp_hier", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP generation function
* \param [in] unit - The unit number.
* \param [in] number_of_ecmps - The number of ecmps in the test that needed to be generated.
* \param [in]- *l3_ecmp_info - an ECMP object array - this array has number_of_ecmps entries, each one for
*                               a ECMP structure
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/

static shr_error_e
dnx_apt_l3_ecmp_generate(
    int unit,
    int number_of_ecmps,
    dnx_apt_l3_ecmp_info_t * l3_ecmp_info)
{
    int member_iter;
    int ecmp_iter;
    int hier_iter;
    int nof_fecs;
    int hierarchy_flag;
    int base_fec_id = 0;
    int nof_rpf_modes = 3;
    bcm_l3_ecmp_urpf_mode_t rpf_mode = bcmL3EcmpUrpfInterfaceDefault;
    uint32 fwd_flags[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
        { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };

    SHR_FUNC_INIT_VARS(unit);
    for (hier_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hier_iter++)
    {
        SHR_IF_ERR_EXIT(get_fec_range(unit, hier_iter, &base_fec_id, &nof_fecs));

        for (member_iter = 0; member_iter < NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST; member_iter++)
        {
            intf_array_by_hier[hier_iter][member_iter] = base_fec_id++;
        }
    }
    for (ecmp_iter = 0; ecmp_iter < number_of_ecmps; ecmp_iter++)
    {
        bcm_l3_egress_ecmp_t_init(&l3_ecmp_info[ecmp_iter].ecmp_struct);
        l3_ecmp_info[ecmp_iter].count_intf = (ecmp_iter % NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST) + 1;
        l3_ecmp_info[ecmp_iter].ecmp_id = ecmp_iter + 1;
        hier_iter =
            (((ecmp_iter +
               1) / dnx_data_l3.ecmp.ecmp_bank_size_get(unit)) %
             dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit));
        hierarchy_flag = fwd_flags[hier_iter];
        l3_ecmp_info[ecmp_iter].fec_ids = intf_array_by_hier[hier_iter];
        l3_ecmp_info[ecmp_iter].ecmp_struct.flags = BCM_L3_WITH_ID | hierarchy_flag;
        l3_ecmp_info[ecmp_iter].ecmp_struct.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
        l3_ecmp_info[ecmp_iter].ecmp_struct.ecmp_intf = ecmp_iter + 1;
        /*
         * adding different rpf modes in order to increase the number of profiles
         */
        l3_ecmp_info[ecmp_iter].ecmp_struct.rpf_mode = rpf_mode;
        if (l3_ecmp_info[ecmp_iter].count_intf == NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST)
        {
            rpf_mode = (rpf_mode + 1) % nof_rpf_modes;
        }

        /** If the ECMP group is in the extended range, add the needed flag. the -1 is since  ecmp_idx is greater than ecmp_iter by 1*/
        if (ecmp_iter > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit) - 1)
        {
            l3_ecmp_info[ecmp_iter].ecmp_struct.ecmp_group_flags = BCM_L3_ECMP_EXTENDED;
        }

    }
exit:
    SHR_FUNC_EXIT;

}
/**
* \brief
*  L3 ECMP FEC generation function
* \param [in] unit - The unit number.
* \param [in] number_of_fecs - The number of FECs that will be generated
* \param [in]- *l3_fec_info - an two dimentional array with number of fecs ecmp hierarchies rows and number of fecs columns
*                             the fecs are generated and saved in this array.
* \return
*   \retval none

* \see
*/
static void
dnx_apt_l3_fec_generate(
    int unit,
    int number_of_fecs,
    int number_of_hierarchies,
    dnx_apt_l3_fec_info_t * l3_fec_info[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES])
{
    int hier_iter = 0;
    int fec_iter = 0;

    uint32 fwd_flags[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
        { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };
    bcm_port_t port = 200;

    for (hier_iter = 0; hier_iter < number_of_hierarchies; hier_iter++)
    {
        for (fec_iter = 0; fec_iter < number_of_fecs; fec_iter++)
        {

            bcm_l3_egress_t_init(&(l3_fec_info[hier_iter][fec_iter].fec_struct));
            l3_fec_info[hier_iter][fec_iter].fec_struct.flags |= fwd_flags[hier_iter];
            l3_fec_info[hier_iter][fec_iter].fec_struct.intf = 100;
            l3_fec_info[hier_iter][fec_iter].fec_struct.port = port;
            l3_fec_info[hier_iter][fec_iter].fec_id = intf_array_by_hier[hier_iter][fec_iter];
            l3_fec_info[hier_iter][fec_iter].flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
            bcm_l3_egress_create(unit, l3_fec_info[hier_iter][fec_iter].flags,
                                 &(l3_fec_info[hier_iter][fec_iter].fec_struct),
                                 &(l3_fec_info[hier_iter][fec_iter].fec_id));
        }
    }
}

/**
* \brief
*  L3 ECMP init function, called in the pre-execute stage of Inserd/Update/Destroy APT test.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- **custom_data - its the structure of the APT test - casting to dnx_apt_l3_ecmp_info_t ** is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e

* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_multiply_and_divide_mem_alloc
                    (unit, number_of_calls, (dnx_apt_l3_ecmp_info_t **) custom_data));
    dnx_apt_l3_ecmp_generate(unit, number_of_calls, (dnx_apt_l3_ecmp_info_t *) * custom_data);
    dnx_apt_l3_fec_generate(unit, NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST,
                            dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit), l3_fec_info);
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP add exec function, called in the execute stage of Insert APT test.
*  This function creating the ECMP in the current call_iter.
* \param [in] unit - The unit number.
* \param [in] call_iter - The current call_iter of the APT test.
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_add_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);
    cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[call_iter];
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &(cur_ecmp->ecmp_struct), cur_ecmp->count_intf, cur_ecmp->fec_ids));
exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  L3 ECMP get exec function, called in the execute stage of get APT test.
*  This function calls to bcm_l3_egress_ecmp_get of the ECMP in the current call_iter.
* \param [in] unit - The unit number.
* \param [in] call_iter - The current call_iter of the APT test.
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_get_exec(
    int unit,
    int call_iter,
    void *custom_data)
{

    bcm_l3_egress_ecmp_t get_ecmp;
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    bcm_if_t intf_array[NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST];
    int intf_count = 0;

    SHR_FUNC_INIT_VARS(unit);
    cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[call_iter];
    get_ecmp.ecmp_intf = cur_ecmp->ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &get_ecmp, NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST, intf_array,
                                           &intf_count));

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  L3 ECMP destroy function, called in the destroy stage of DESTROY APT test.
*  This function destroys the ECMP in the current call_iter.
*  This function is called fo each ECMP.
* \param [in] unit - The unit number.
* \param [in] call_iter - The current call_iter of the APT test.
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_destroy_exec(
    int unit,
    int call_iter,
    void *custom_data)
{
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);
    cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[call_iter];
    cur_ecmp->ecmp_struct.flags &= ~BCM_L3_REPLACE;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &(cur_ecmp->ecmp_struct)));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP add function, called in the pre-excute stage of DESTROY APT test.
*  This function creates all of the ECMPs in the test.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_add_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{

    int ecmp_iter = 0;
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);

    for (ecmp_iter = 0; ecmp_iter < number_of_calls; ecmp_iter++)
    {
        cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[ecmp_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create
                        (unit, &(cur_ecmp->ecmp_struct), cur_ecmp->count_intf, cur_ecmp->fec_ids));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP APT function, called in the pre-excute stage of UPDATE APT test.
*  This function creates all of the ECMPs in the test and changes their number of members as a preperation for update stage.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_update_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int ecmp_iter = 0;
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);

    for (ecmp_iter = 0; ecmp_iter < number_of_calls; ecmp_iter++)
    {
        cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[ecmp_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create
                        (unit, &(cur_ecmp->ecmp_struct), cur_ecmp->count_intf, cur_ecmp->fec_ids));
        cur_ecmp->ecmp_struct.flags |= BCM_L3_REPLACE;
        cur_ecmp->count_intf = ((cur_ecmp->count_intf) % (NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST - 1) + 1);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP clear function, called in the post-excute stage of UPDATE APT test.
*  This function destroys all of the ECMPs in the test and clear its flags for the next APT test run.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_update_clear(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int call_iter;
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);
    for (call_iter = 0; call_iter < number_of_calls; call_iter++)
    {
        cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[call_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &(cur_ecmp->ecmp_struct)));
        cur_ecmp->ecmp_struct.flags &= ~BCM_L3_REPLACE;
        cur_ecmp->ecmp_struct.rpf_mode = bcmL3EcmpUrpfInterfaceDefault;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  L3 ECMP clear function, called in the post-excute stage of Insert APT test.
*  This function destroys all of the ECMPs in the test and clear its flags for the next APT test run.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_clear(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    int call_iter;
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);
    for (call_iter = 0; call_iter < number_of_calls; call_iter++)
    {
        cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[call_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &(cur_ecmp->ecmp_struct)));
        cur_ecmp->ecmp_struct.rpf_mode = bcmL3EcmpUrpfInterfaceDefault;

    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP free memory function, called in the the end of al APT ECMP tests.
*  This function free all the memory that was allocated for ecmp structures and all of the fec allocated memory.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in] number_of_hierarchies - The number of hierarchies of the ECMPs in the test - this number is changing according to the test type
*                                     Consistent hashing-1
*                                     Multiply and divide - 3
*
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_mem_free(
    int unit,
    int number_of_calls,
    int number_of_hierarchies,
    void *custom_data)
{
    int hier_iter;
    int fec_iter;
    SHR_FUNC_INIT_VARS(unit);
    for (hier_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1; hier_iter < number_of_hierarchies; hier_iter++)
    {
        for (fec_iter = 0; fec_iter < NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST; fec_iter++)
        {
            BCM_L3_ITF_SET(intf_array_by_hier[hier_iter][fec_iter], BCM_L3_ITF_TYPE_FEC,
                           BCM_L3_ITF_VAL_GET(intf_array_by_hier[hier_iter][fec_iter]));
            SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, intf_array_by_hier[hier_iter][fec_iter]));
        }
    }
    for (hier_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1; hier_iter < number_of_hierarchies; hier_iter++)
    {
        SHR_FREE(intf_array_by_hier[hier_iter]);
        SHR_FREE(l3_fec_info[hier_iter]);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP multiply_and_divide mem free, calling to dnx_apt_l3_ecmp_mem_free
*  used for sending number of hierarcies for fec destroy function, in multiply_and_divide test works with all hierarchies.
*  */
static shr_error_e
dnx_apt_l3_ecmp_multiply_and_divide_mem_free(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_mem_free
                    (unit, number_of_calls, dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit), custom_data));
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

/*
 * Test Objects
 */
const dnx_apt_object_def_t dnx_apt_l3_ecmp_multiply_and_divide_insert_object = {
    /** Test name */
    "L3_ECMP_multiply_and_divide_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when inserting new non-consecutive ECMP entries",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_insert_multiply_and_divide_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_multiply_and_divide_mem_free
};

const dnx_apt_object_def_t dnx_apt_l3_ecmp_multiply_and_divide_update_object = {
    /** Test name */
    "L3_ECMP_multiply_and_divide_Update",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when updating existing non-consecutive ECMP entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_update_multiply_and_divide_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_update_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_update_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_multiply_and_divide_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_multiply_and_divide_get_object = {
    /** Test name */
    "L3_ECMP_multiply_and_divide_get",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_get() when geting existing ECMP entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_get_multiply_and_divide_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_add_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_get_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_multiply_and_divide_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_multiply_and_divide_destroy_object = {
    /** Test name */
    "L3_ECMP_multiply_and_divide_Destroy",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_destroy() when deleting existing non-consecutive ECMP entries. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_destroy_multiply_and_divide_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_add_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_multiply_and_divide_mem_free
};

/*
 * }
 */

/*
 * L3 ECMP consistent hashing functions
 */

/**
* \brief
*  L3 ECMP consistent hashing single ECMP initialize function , called as part of the ECMP consistent hashing initializing.
*  This function initialize single ECMP structure and updating its fields before creating the ECMP.
* \param [in] l3_ecmp_info - the ECMP info structure array
* \param [in] ecmp_idx - the index of the current ECMP that is initialized
* \param [in] max_paths - the max number of members for current ECMP
* \param [in] count_intf - the actual number of members for current ECMP
* \param [in] hierarchy - the FEC ECMP hierarchy of the current ECMP
* \param [in] flags - flags for the ECMP, used for forwarding hierarchy_flag
* \param [in] ecmp_group_flags - flags for ECMP table size, used to forward BCM_L3_ECMP_LARGE_TABLE flag for large table, zero otherwise.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static void
dnx_apt_l3_ecmp_consistent_hashing_init_single_ecmp(
    int unit,
    dnx_apt_l3_ecmp_info_t * l3_ecmp_info,
    int ecmp_idx,
    int max_paths,
    int count_intf,
    int hierarchy,
    uint32 flags,
    uint32 ecmp_group_flags)
{
    bcm_l3_egress_ecmp_t_init(&l3_ecmp_info[ecmp_idx].ecmp_struct);
    l3_ecmp_info[ecmp_idx].ecmp_struct.ecmp_group_flags = ecmp_group_flags;
    l3_ecmp_info[ecmp_idx].ecmp_struct.max_paths = max_paths;
    l3_ecmp_info[ecmp_idx].count_intf = count_intf;
    l3_ecmp_info[ecmp_idx].fec_ids = intf_array_by_hier[hierarchy];
    l3_ecmp_info[ecmp_idx].ecmp_struct.flags = BCM_L3_WITH_ID | flags;
    l3_ecmp_info[ecmp_idx].ecmp_struct.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    l3_ecmp_info[ecmp_idx].ecmp_struct.ecmp_intf =
        (hierarchy * dnx_data_l3.ecmp.ecmp_bank_size_get(unit)) + ecmp_idx + 1;
    l3_ecmp_info[ecmp_idx].ecmp_id = ecmp_idx + 1;

    /** If the ECMP group is in the extended range, add the needed flag. the -1 is since  ecmp_idx is greater than ecmp_iter by 1*/
    if (ecmp_idx > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit) - 1)

    {
        l3_ecmp_info[ecmp_idx].ecmp_struct.ecmp_group_flags |= BCM_L3_ECMP_EXTENDED;
    }
}
/**
* \brief
*  L3 ECMP consistent hashing allocation function
* \param [in] unit - The unit number.
* \param [in] number_of_ecmps - The number of ecmps in the test that needed to be allocated.
* \param [in]- **l3_ecmp_info - a pointer to ecmps object array - this array has number_of_ecmps entries, each one for
*                               a ecmp structure
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_alloc(
    int unit,
    int number_of_ecmps,
    dnx_apt_l3_ecmp_info_t ** l3_ecmp_info)
{
    int hier_iter = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(*l3_ecmp_info, (sizeof(dnx_apt_l3_ecmp_info_t) * number_of_ecmps), "dnx_apt_l3_ecmp_info_t",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(intf_array_by_hier[hier_iter], (sizeof(bcm_if_t) * NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST),
              "dnx_apt_l3_ecmp_hier", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(l3_fec_info[hier_iter], (sizeof(dnx_apt_l3_fec_info_t) * NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST),
              "dnx_apt_l3_ecmp_hier", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP generation function for consistent hashing.
* \param [in] unit - The unit number.
* \param [in] number_of_ecmps - The number of ECMPs that will be generated
* \param [in]- *l3_ecmp_info - an two dimentional array with number of fecs ecmp hierarchies rows and number of fecs columns
* \param [in]- *table_size - table size for consistent hashing
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_generate(
    int unit,
    int number_of_ecmps,
    dnx_apt_l3_ecmp_info_t * l3_ecmp_info,
    int table_size)
{
    int nof_fecs;
    int hierarchy_flag;
    int base_fec_id = 0;
    int hier_iter = 0;
    int ecmp_iter = 0;
    int member_iter = 0;
    int num_of_consistent_tables = 0;
    int max_paths = 0;
    uint32 ecmp_group_flags = 0;
    uint32 fwd_flags[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
        { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };
    SHR_FUNC_INIT_VARS(unit);
    num_of_consistent_tables =
        L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit) / L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, table_size);
    SHR_IF_ERR_EXIT(get_fec_range(unit, hier_iter, &base_fec_id, &nof_fecs));
    for (member_iter = 0; member_iter < NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST; member_iter++)
    {
        intf_array_by_hier[hier_iter][member_iter] = base_fec_id++;
    }

    hier_iter = 0;
    hierarchy_flag = fwd_flags[hier_iter];
    switch (table_size)
    {
        case APT_ECMP_CONSISTENT_HASING_WITH_SMALL_TABLES:
        {
            /*
             * Creating ECMP with unique tables as much as possible - the -2 is for the ability to change all tables
             * while updating
             */
            max_paths = 15;
            for (ecmp_iter = 0; ecmp_iter < num_of_consistent_tables - 2; ecmp_iter++)
            {
                dnx_apt_l3_ecmp_consistent_hashing_init_single_ecmp(unit, l3_ecmp_info, ecmp_iter, 15, 14, hier_iter,
                                                                    hierarchy_flag, 0);
            }

            break;
        }
        case APT_ECMP_CONSISTENT_HASING_WITH_MEDIUM_TABLES:
        {
            max_paths = 250;
            for (ecmp_iter = 0; ecmp_iter < num_of_consistent_tables - 2; ecmp_iter++)
            {
                dnx_apt_l3_ecmp_consistent_hashing_init_single_ecmp(unit, l3_ecmp_info, ecmp_iter, 250, 230, hier_iter,
                                                                    hierarchy_flag, 0);
            }
            break;
        }
        case APT_ECMP_CONSISTENT_HASING_WITH_LARGE_TABLES:
        {
            max_paths = 250;
            ecmp_group_flags = BCM_L3_ECMP_LARGE_TABLE;

            for (ecmp_iter = 0; ecmp_iter < num_of_consistent_tables - 2; ecmp_iter++)
            {
                dnx_apt_l3_ecmp_consistent_hashing_init_single_ecmp(unit, l3_ecmp_info, ecmp_iter, 250, 230, hier_iter,
                                                                    hierarchy_flag, ecmp_group_flags);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Unknown consistent table size - %d.\n", table_size);
            break;
        }
    }
    /*
     * Creating the rest of the ECMPS with the same table 
     */

    for (; ecmp_iter < number_of_ecmps; ecmp_iter++)
    {
        dnx_apt_l3_ecmp_consistent_hashing_init_single_ecmp(unit, l3_ecmp_info, ecmp_iter, max_paths, max_paths,
                                                            hier_iter, hierarchy_flag, ecmp_group_flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP consistent hashing initialization function, used as pre-execute function for all ECMP consistent hashing tests
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of ECMPs that will be generated in the test
* \param [in]- **custom_data - its the structure of the APT test - casting to dnx_apt_l3_ecmp_info_t ** is performed.
* \param [in]- table_size_flag - table size for consistent hashing large table size
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/

static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_init(
    int unit,
    int number_of_calls,
    void **custom_data,
    int table_size_flag)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_consistent_hashing_alloc
                    (unit, number_of_calls, (dnx_apt_l3_ecmp_info_t **) custom_data));
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_consistent_hashing_generate
                    (unit, number_of_calls, (dnx_apt_l3_ecmp_info_t *) * custom_data, table_size_flag));
    dnx_apt_l3_fec_generate(unit, NOF_MAX_ECMP_GROUP_MEMBERS_IN_APT_TEST, 1, l3_fec_info);
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing small table initialization function, calling to dnx_apt_l3_ecmp_consistent_hashing_init
*  with the proper table size flag
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- **custom_data - This is the structure of the APT test - casting to dnx_apt_l3_ecmp_info_t ** is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*
*/
static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_small_table_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_consistent_hashing_init(unit, number_of_calls, custom_data,
                                                            APT_ECMP_CONSISTENT_HASING_WITH_SMALL_TABLES));
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing medium table initialization function, calling to dnx_apt_l3_ecmp_consistent_hashing_init
*  with the proper table size flag
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- **custom_data - This is the structure of the APT test - casting to dnx_apt_l3_ecmp_info_t ** is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*
*/
static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_medium_table_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_consistent_hashing_init(unit, number_of_calls, custom_data,
                                                            APT_ECMP_CONSISTENT_HASING_WITH_MEDIUM_TABLES));
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing large table initialization function, calling to dnx_apt_l3_ecmp_consistent_hashing_init
*  with the proper table size flag
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- **custom_data - This is the structure of the APT test - casting to dnx_apt_l3_ecmp_info_t ** is performed.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_large_table_init(
    int unit,
    int number_of_calls,
    void **custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_consistent_hashing_init(unit, number_of_calls, custom_data,
                                                            APT_ECMP_CONSISTENT_HASING_WITH_LARGE_TABLES));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  L3 ECMP APT function, called in the pre-excute stage of UPDATE ECMP consistent hashing APT test.
*  This function creates all of the ECMPs in the test and changes their number of members before update stage.
* \param [in] unit - The unit number.
* \param [in] number_of_calls - The number of calls of the APT test
* \param [in]- *custom_data - its a structure of the APT test - casting to dnx_apt_l3_ecmp_info_t * is performed.
* \param [in]- count_intf - The nof members the ECMPs with unique tables will have
* \param [in]- max_paths  - The max nof members for all of the ECMPs
* \param [in]- table_size - flag indicating table size - small/medium/large.
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
* \see
*/
static shr_error_e
dnx_apt_l3_ecmp_update_consistent_hashing_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data,
    int count_intf,
    int max_paths,
    int table_size)
{
    int ecmp_iter = 0;
    int num_of_consistent_tables = 0;
    dnx_apt_l3_ecmp_info_t *cur_ecmp;
    SHR_FUNC_INIT_VARS(unit);
    num_of_consistent_tables =
        L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit) / L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, table_size);

    /*
     * updating ecmps with new table as much as we can 
     */
    for (ecmp_iter = number_of_calls - 1; ecmp_iter > number_of_calls - (num_of_consistent_tables - 2); ecmp_iter--)
    {
        cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[ecmp_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create
                        (unit, &(cur_ecmp->ecmp_struct), cur_ecmp->count_intf, cur_ecmp->fec_ids));
        cur_ecmp->ecmp_struct.flags |= BCM_L3_REPLACE;
        cur_ecmp->count_intf = count_intf;
        cur_ecmp->ecmp_struct.max_paths = max_paths;
        if (table_size == APT_ECMP_CONSISTENT_HASING_WITH_LARGE_TABLES)
        {
            cur_ecmp->ecmp_struct.ecmp_group_flags |= BCM_L3_ECMP_LARGE_TABLE;
        }
    }

    /*
     * updating the rest of the ecmps with the same new updated table
     */
    for (; ecmp_iter >= 0; ecmp_iter--)
    {
        cur_ecmp = &((dnx_apt_l3_ecmp_info_t *) custom_data)[ecmp_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create
                        (unit, &(cur_ecmp->ecmp_struct), cur_ecmp->count_intf, cur_ecmp->fec_ids));
        cur_ecmp->ecmp_struct.flags |= BCM_L3_REPLACE;
        cur_ecmp->count_intf = max_paths;
        cur_ecmp->ecmp_struct.max_paths = max_paths;
        if (table_size == APT_ECMP_CONSISTENT_HASING_WITH_LARGE_TABLES)
        {
            cur_ecmp->ecmp_struct.ecmp_group_flags |= BCM_L3_ECMP_LARGE_TABLE;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing small table pre-update function, calling to dnx_apt_l3_ecmp_update_pre_exec
*  with the proper count_intf and max_paths parameters
*/
static shr_error_e
dnx_apt_l3_ecmp_update_consistent_hashing_small_table_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_update_consistent_hashing_pre_exec
                    (unit, number_of_calls, custom_data, 12, 13, APT_ECMP_CONSISTENT_HASING_WITH_SMALL_TABLES));
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing medium table pre-update function, calling to dnx_apt_l3_ecmp_update_pre_exec
*  with the proper count_intf and max_paths parameters
*/ static shr_error_e
dnx_apt_l3_ecmp_update_consistent_hashing_medium_table_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_update_consistent_hashing_pre_exec
                    (unit, number_of_calls, custom_data, 240, 245, APT_ECMP_CONSISTENT_HASING_WITH_MEDIUM_TABLES));
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing largetable pre-update function, calling to dnx_apt_l3_ecmp_update_pre_exec
*  with the proper count_intf and max_paths parameters
*/ static shr_error_e
dnx_apt_l3_ecmp_update_consistent_hashing_large_table_pre_exec(
    int unit,
    int number_of_calls,
    void *custom_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_update_consistent_hashing_pre_exec
                    (unit, number_of_calls, custom_data, 240, 245, APT_ECMP_CONSISTENT_HASING_WITH_LARGE_TABLES));
exit:
    SHR_FUNC_EXIT;
}
/*
* \brief
*  L3 ECMP consistent hashing mem free, calling to dnx_apt_l3_ecmp_update_pre_exec
*  used for sending number of hierarcies for fec destroy function, in consistent hashing test works with 1 hierarchy only.
*  */
static shr_error_e
dnx_apt_l3_ecmp_consistent_hashing_mem_free(
    int unit,
    int number_of_calls,
    void *custom_data)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_apt_l3_ecmp_mem_free(unit, number_of_calls, 1, custom_data));
exit:
    SHR_FUNC_EXIT;
}
/*
 * ECMP consistent hashing test objects
 */
const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_small_table_insert_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Small_Table_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when inserting new non-consecutive ECMP entries with small tables consistent hashing",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_insert_consistent_hashing_small_table_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_small_table_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_medium_table_insert_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Medium_Table_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when inserting new non-consecutive ECMP entries with medium tables consistent hashing",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_insert_consistent_hashing_medium_table_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_medium_table_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_large_table_insert_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Large_Table_Insert",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when inserting new non-consecutive ECMP entries with large tables consistent hashing",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_insert_consistent_hashing_large_table_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_large_table_init,
    /** Run init procedure */
    NULL,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};

const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_small_table_update_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Small_Table_Update",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when updating new non-consecutive ECMP entries with small tables consistent hashing",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_update_consistent_hashing_small_table_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_small_table_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_update_consistent_hashing_small_table_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_update_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_medium_table_update_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Medium_Table_Update",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when updating new non-consecutive ECMP entries with medium tables consistent hashing",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_update_consistent_hashing_medium_table_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_medium_table_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_update_consistent_hashing_medium_table_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_update_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_large_table_update_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Large_Table_Update",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_create() when updating new non-consecutive ECMP entries with large tables consistent hashing",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_update_consistent_hashing_large_table_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_large_table_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_update_consistent_hashing_large_table_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_add_exec,
    /** Run deinit procedure */
    dnx_apt_l3_ecmp_update_clear,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};
const dnx_apt_object_def_t dnx_apt_l3_ecmp_consistent_hashing_destroy_object = {
    /** Test name */
    "L3_ECMP_Consistent_Hashing_Destroy",
    /** Test description */
    "Measures the performance of bcm_l3_egress_ecmp_destroy() when deleting existing non-consecutive ECMP entries with consistent hashing. ",
    /** Support flags */
    0,
    /** ctest flags */
    CTEST_POSTCOMMIT,
    /** Validation method */
    DNX_APT_VALIDATION_METHOD_AVERAGE_TRIM_WORST_AND_BEST,
    /** Number of runs */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_RUNS,
    /** Number of calls */
    DNX_APT_L3_ECMP_DEFAULT_NUMBER_OF_CALLS,
    /** DDMA table */
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES,
    /** Time threshold */
    dnx_apt_l3_ecmp_destroy_consistent_hashing_time_threshold,
    /** Optional threshold info */
    NULL,
    /** Custom support check procedure */
    NULL,
    /** SOC properties set procedure */
    NULL,
    /** Test init procedure */
    dnx_apt_l3_ecmp_consistent_hashing_small_table_init,
    /** Run init procedure */
    dnx_apt_l3_ecmp_add_pre_exec,
    /** Test execution procedure */
    dnx_apt_l3_ecmp_destroy_exec,
    /** Run deinit procedure */
    NULL,
    /** Test deinit procedure */
    dnx_apt_l3_ecmp_consistent_hashing_mem_free
};
