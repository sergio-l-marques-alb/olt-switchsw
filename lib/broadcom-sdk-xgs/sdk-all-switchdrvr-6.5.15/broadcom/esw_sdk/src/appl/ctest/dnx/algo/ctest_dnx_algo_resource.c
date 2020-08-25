/** \file resource_manager_unit_test.c
 *
 * Resource management unit test.
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

/**
* INCLUDE FILES:
* {
*/

#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/access/example_res_mngr_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>

#include "ctest_dnx_algo_resource.h"

/**
 * }
 */

/*
 * DEFINES
 */

/*
 * used when there is no need for flags
 * for example in API sw_state_algo_res_allocate
 */
#define FLAGS_NONE 0

/*
 * used to define first element and nof elements in resource manager
 */
#define RESOURCE_FIRST_ELEMENT 0
#define RESOURCE_NOF_ELEMENTS  200

/*
 * core IDs
 */
#define FIRST_CORE_ID  0
#define SECOND_CORE_ID 1
#define MIN_NOF_CORES  2

/*
 * Poll IDs
 */
#define FIRST_SUB_RESOURCE_INDX 0
#define SECOND_SUB_RESOURCE_INDX 1
#define NOF_SUB_RESOURCES 2

/*
 * illegal values
 */
#define ILLEGAL_NOF_ELEMENTS 0
#define ILLEGAL_CORE_ID -1
#define ILLEGAL_ELEMENT_ID -1

/*
 * Resource names.
 */
#define RESOURCE_TEST_INPUT_VALID_CORE_ALL  "resource_test_input_valid_core_all"
#define RESOURCE_TEST_INPUT_VALID_PER_CORE  "resource_test_input_valid_per_core"
#define RESOURCE_TEST_INPUT_VALID_INDEXED_RESOURCE "resource_test_input_valid_indexed_resource"
#define RESOURCE_TEST_INPUT_VALID_INDEXED_RESOURCE_PER_CORE "resource_test_input_valid_indexed_resource_per_core"

#define RESOURCE_TEST_CORE_ALL  "resource_test_core_all"

#define RESOURCE_TEST_PER_CORE  "resource_test_per_core"

#define RESOURCE_TEST_INVALID   "resource_test_invalid"

#define RESOURCE_TEST_INDEXED_RESOURCE "resource_test_indexed_resource"

#define RESOURCE_TEST_INDEXED_RESOURCE_PER_CORE "resource_test_indexed_resource_per_core"



/*
 * structs to support bcm shell command
 */
/*
 * {
 */
sh_sand_option_t dnx_resource_manager_test_options[] = {
    {NULL}
};

sh_sand_man_t dnx_resource_manager_test_man = {
    "Unit Test for resource Manager",
    "Unit Test for resource Manager, tests all valid/invalud input values for each API, and runs scenario to check the overall API behavior"
};

/*
 * }
 */

/**
 * \brief
 * runs resource manager test scenario for cases where we create resource manager per core
 * the function will run only in case nof_cores > 1
 * test scenario :
 * create resource manager per core and do the following:
 * 1 - check that all elements are free in both cores 0 and 1
 * 2 - allocate all elements in core 0
 * 3 - check is_allocated in both cores
 * 4 - allocate all elements in core 1
 * 5 - check is_allocated in both cores
 * 6 - free all elements in core 0
 * 7 - check is_allocated in both cores
 * 8 - free all elements in core 1
 * 9 - check is_allocated in both cores
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *   NONE
 * \par INDIRECT OUTPUT
 *   the executed test
 */
static shr_error_e
dnx_algo_res_per_core_scenario(
    int unit)
{

    int element, element_iterator, sub_resource_indx;
    uint8 is_allocated;
    uint32 core_id;
    sw_state_algo_res_create_data_t data_per_core, data_per_core_indexed_resource;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data_per_core, 0, sizeof(sw_state_algo_res_create_data_t));
    data_per_core.flags = SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE;
    data_per_core.first_element = RESOURCE_FIRST_ELEMENT;
    data_per_core.nof_elements = RESOURCE_NOF_ELEMENTS;

    sal_memset(&data_per_core_indexed_resource, 0, sizeof(sw_state_algo_res_create_data_t));
    data_per_core_indexed_resource.flags =
        SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE | SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data_per_core_indexed_resource.first_element = RESOURCE_FIRST_ELEMENT;
    data_per_core_indexed_resource.nof_elements = RESOURCE_NOF_ELEMENTS;
    data_per_core_indexed_resource.nof_resource_pool_indexes = NOF_SUB_RESOURCES;
    sal_strncpy(data_per_core_indexed_resource.name, RESOURCE_TEST_INDEXED_RESOURCE_PER_CORE,
                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.alloc
                    (unit, SW_STATE_ALGO_RES_NOF_CORES_GET(unit), NOF_SUB_RESOURCES));
    for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
    {
        for (sub_resource_indx = 0; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.create
                            (unit, core_id, sub_resource_indx, &data_per_core_indexed_resource, NULL));
        }
    }

    sal_strncpy(data_per_core.name, RESOURCE_TEST_PER_CORE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.alloc(unit, SW_STATE_ALGO_RES_NOF_CORES_GET(unit)));
    for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.create(unit, core_id, &data_per_core, NULL));
    }

    /*
     * iterate over all elements in both cores and check that all of them are free
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, FIRST_CORE_ID, element_iterator, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element is not allocated, iteration : %d\n",
                         element_iterator);
        }
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated(unit, SECOND_CORE_ID, element_iterator,
                                                                             &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element is not allocated, iteration : %d\n",
                         element_iterator);
        }
    }

    /*
     * iterate over all elements in both cores and all sub resources and check that all of them are free
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, FIRST_CORE_ID, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element is not allocated, iteration : %d\n",
                             element_iterator);
            }
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, SECOND_CORE_ID, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element is not allocated, iteration : %d\n",
                             element_iterator);
            }
        }
    }

    /*
     * allocate all elements in core 0 and check that they were not allocated also in core 1
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.allocate_single
                        (unit, FIRST_CORE_ID, FLAGS_NONE, NULL, &element));

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, FIRST_CORE_ID, element, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was successful but is_allocated = false, iteration : %d\n",
                         element_iterator);
        }

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, SECOND_CORE_ID, element, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was only for core = 0, iteration : %d\n", element_iterator);
        }
    }

    /*
     * allocate all elements in core 0(in all sub resources) and check that they were not allocated also in core 1
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.allocate_single
                            (unit, FIRST_CORE_ID, sub_resource_indx, FLAGS_NONE, NULL, &element));

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, FIRST_CORE_ID, sub_resource_indx, element, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was successful but is_allocated = false, iteration : %d\n",
                             element_iterator);
            }

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, SECOND_CORE_ID, sub_resource_indx, element, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was only for core = 0, iteration : %d\n", element_iterator);
            }
        }
    }

    /*
     * allocate all elements in core 1 and check that now they are allocated in both cores
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.allocate_single
                        (unit, SECOND_CORE_ID, FLAGS_NONE, NULL, &element));

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, FIRST_CORE_ID, element, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "element should be already allocated in core 0, iteration : %d\n",
                         element_iterator);
        }

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, SECOND_CORE_ID, element, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was successful but is_allocated = false, iteration : %d\n",
                         element_iterator);
        }
    }

    /*
     * allocate all elements in core 1(in all sub resources) and check that now they are allocated in both cores
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.allocate_single
                            (unit, SECOND_CORE_ID, sub_resource_indx, FLAGS_NONE, NULL, &element));

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, FIRST_CORE_ID, sub_resource_indx, element, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "element should be already allocated in core 0, iteration : %d\n",
                             element_iterator);
            }

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, SECOND_CORE_ID, sub_resource_indx, element, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was successful but is_allocated = false, iteration : %d\n",
                             element_iterator);
            }
        }
    }

    /*
     * free all elements in core 0, check that they were not freed in core 1 as well
     */

    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.clear(unit, FIRST_CORE_ID));

    element_iterator = RESOURCE_FIRST_ELEMENT;
    while (TRUE)
    {

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.get_next(unit, FIRST_CORE_ID, &element_iterator));

        if (element_iterator == SW_STATE_ALGO_RES_ILLEGAL_ELEMENT)
        {
            break;
        }

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, FIRST_CORE_ID, element_iterator, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, iteration : %d\n", element_iterator);
        }

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated(unit, SECOND_CORE_ID, element_iterator,
                                                                             &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "free was only for core = 0, iteration : %d\n", element_iterator);
        }

        element_iterator++;
    }

    /*
     * free all elements in core 0(from all sub resources), check that they were not freed in core 1 as well
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {

        element_iterator = RESOURCE_FIRST_ELEMENT;
        while (TRUE)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.get_next
                            (unit, FIRST_CORE_ID, sub_resource_indx, &element_iterator));

            if (element_iterator == SW_STATE_ALGO_RES_ILLEGAL_ELEMENT)
            {
                break;
            }

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.free_single
                            (unit, FIRST_CORE_ID, sub_resource_indx, element_iterator));

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, FIRST_CORE_ID, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, iteration : %d\n",
                             element_iterator);
            }

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, SECOND_CORE_ID, sub_resource_indx, element_iterator, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "free was only for core = 0, iteration : %d\n", element_iterator);
            }

            element_iterator++;
        }

    }

    /*
     * free all elements in core 1, check that now they are free in both cores
     */
    element_iterator = RESOURCE_FIRST_ELEMENT;
    while (TRUE)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.get_next(unit, FIRST_CORE_ID, &element_iterator));

        if (element_iterator == SW_STATE_ALGO_RES_ILLEGAL_ELEMENT)
        {
            break;
        }

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.free_single(unit, SECOND_CORE_ID, element_iterator));

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, FIRST_CORE_ID, element_iterator, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, iteration : %d\n", element_iterator);
        }

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_per_core.is_allocated
                        (unit, SECOND_CORE_ID, element_iterator, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, iteration : %d\n", element_iterator);
        }

        element_iterator++;
    }

    /*
     * free all elements in core 1(from all sub resources), check that now they are free in both cores
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        element_iterator = RESOURCE_FIRST_ELEMENT;
        while (TRUE)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.get_next
                            (unit, FIRST_CORE_ID, sub_resource_indx, &element_iterator));

            if (element_iterator == SW_STATE_ALGO_RES_ILLEGAL_ELEMENT)
            {
                break;
            }
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.free_single
                            (unit, SECOND_CORE_ID, sub_resource_indx, element_iterator));

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, FIRST_CORE_ID, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, iteration : %d\n",
                             element_iterator);
            }

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource_per_core.is_allocated
                            (unit, SECOND_CORE_ID, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, iteration : %d\n",
                             element_iterator);
            }

            element_iterator++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * runs resource manager test scenario for cases where we create one resource manager for all cores
 * test scenario :
 * create resource manager and do the following:
 * 1 - check that all elements are free
 * 2 - allocate all elements
 * 3 - check is_allocated per element
 * 4 - allocate one more element - should fail
 * 5 - free all elements
 * 6 - check is_allocated per element
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *   NONE
 * \par INDIRECT OUTPUT
 *   the executed test
 */
static shr_error_e
dnx_algo_res_core_all_scenario(
    int unit)
{

    int element, element_iterator;
    int sub_resource_indx;
    shr_error_e rv;
    uint8 is_allocated;
    uint32 sub_resource_id;
    sw_state_algo_res_create_data_t data_all_cores, data_indexed_resource;
    int nof_elements = 0;

    SHR_FUNC_INIT_VARS(unit);

    rv = _SHR_E_NONE;

    sal_memset(&data_all_cores, 0, sizeof(sw_state_algo_res_create_data_t));
    data_all_cores.flags = FLAGS_NONE;
    data_all_cores.first_element = RESOURCE_FIRST_ELEMENT;
    data_all_cores.nof_elements = RESOURCE_NOF_ELEMENTS;
    sal_strncpy(data_all_cores.name, RESOURCE_TEST_CORE_ALL, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    sal_memset(&data_indexed_resource, 0, sizeof(sw_state_algo_res_create_data_t));
    data_indexed_resource.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data_indexed_resource.first_element = RESOURCE_FIRST_ELEMENT;
    data_indexed_resource.nof_elements = RESOURCE_NOF_ELEMENTS;
    data_indexed_resource.nof_resource_pool_indexes = NOF_SUB_RESOURCES;
    sal_strncpy(data_indexed_resource.name, RESOURCE_TEST_INDEXED_RESOURCE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_core_all.create(unit, &data_all_cores, NULL));

    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.alloc(unit, NOF_SUB_RESOURCES));
    for (sub_resource_id = 0; sub_resource_id < NOF_SUB_RESOURCES; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.create
                        (unit, sub_resource_id, &data_indexed_resource, NULL));
    }

    /*
     * check that all elements are free
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_core_all.is_allocated(unit, element_iterator, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element is not allocated, iteration : %d\n",
                         element_iterator);
        }
    }

    /*
     * check that all elements are free
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.is_allocated
                            (unit, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element is not allocated, iteration : %d\n",
                             element_iterator);
            }
        }
    }

    /*
     * allocate all elements
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_core_all.allocate_single(unit, FLAGS_NONE, NULL, &element));

        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_core_all.is_allocated(unit, element, &is_allocated));
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was successful but is_allocated = false, iteration : %d\n",
                         element_iterator);
        }
    }

    /*
     * allocate all elements
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.allocate_single
                            (unit, sub_resource_indx, FLAGS_NONE, NULL, &element));

            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.is_allocated
                            (unit, sub_resource_indx, element, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "allocation was successful but is_allocated = false, iteration : %d\n",
                             element_iterator);
            }
        }
    }

    /*
     * Check nof_allocated_elements_in_range_get.
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.
                    resource_test_core_all.nof_allocated_elements_in_range_get(unit, 0, 20, &nof_elements));

    if (nof_elements != 20)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "nof_allocated_elements_in_range_get returns wrong value: %d\n", nof_elements);
    }

    /*
     * try to get nof allocated elements with invalid range_start and nof_elements_in_range, should fail.
     */
    rv = algo_res_mngr_db.resource_test_core_all.nof_allocated_elements_in_range_get
        (unit, RESOURCE_NOF_ELEMENTS, 1, &nof_elements);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Range start was bigger than RESOURCE_NOF_ELEMENTS, should fail but it passed\n");
    }

    rv = algo_res_mngr_db.resource_test_core_all.nof_allocated_elements_in_range_get
        (unit, ILLEGAL_ELEMENT_ID, 1, &nof_elements);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Range start was illegal, api should fail but it passed\n");
    }

    rv = algo_res_mngr_db.resource_test_core_all.nof_allocated_elements_in_range_get
        (unit, 0, (RESOURCE_NOF_ELEMENTS + 1), &nof_elements);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Range size was illegal, api should fail but it passed\n");
    }

    /*
     * try to allocate one more element, should fail
     */
    rv = algo_res_mngr_db.resource_test_core_all.allocate_single(unit, FLAGS_NONE, NULL, &element);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "no more resources are available, allocation should fail but it passed\n");
    }

    /*
     * try to allocate one more element in the first pool, should fail
     */
    rv = algo_res_mngr_db.resource_test_indexed_resource.allocate_single
        (unit, FIRST_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "no more resources are available, allocation should fail but it passed\n");
    }

    /*
     * try to allocate one more element in the second pool, should fail
     */
    rv = algo_res_mngr_db.resource_test_indexed_resource.allocate_single
        (unit, SECOND_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "no more resources are available, allocation should fail but it passed\n");
    }

    /*
     * free all elements
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_core_all.free_single(unit, element_iterator));
    }

    /*
     * free all elements
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.free_single
                            (unit, sub_resource_indx, element_iterator));
        }
    }

    /*
     * try to free an extra element, should fail
     */
    rv = algo_res_mngr_db.resource_test_core_all.free_single(unit, RESOURCE_NOF_ELEMENTS - 1);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "element is already free but free was successful\n");
    }
    /*
     * try to free an extra element from first pool, should fail
     */
    rv = algo_res_mngr_db.resource_test_indexed_resource.free_single
        (unit, FIRST_SUB_RESOURCE_INDX, RESOURCE_NOF_ELEMENTS - 1);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "element is already free but free was successful\n");
    }
    /*
     * try to free an extra element from second pool, should fail
     */
    rv = algo_res_mngr_db.resource_test_indexed_resource.free_single
        (unit, SECOND_SUB_RESOURCE_INDX, RESOURCE_NOF_ELEMENTS - 1);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "element is already free but free was successful\n");
    }

    /*
     * make sure is_allocated returns false for all elements
     */
    for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_core_all.is_allocated(unit, element_iterator, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, element: %d\n", element_iterator);
        }
    }

    /*
     * make sure is_allocated returns false for all elements
     */
    for (sub_resource_indx = FIRST_SUB_RESOURCE_INDX; sub_resource_indx < NOF_SUB_RESOURCES; sub_resource_indx++)
    {
        for (element_iterator = RESOURCE_FIRST_ELEMENT; element_iterator < RESOURCE_NOF_ELEMENTS; element_iterator++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_indexed_resource.is_allocated
                            (unit, sub_resource_indx, element_iterator, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "is_allocated = true but element was freed, element: %d\n", element_iterator);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * check input valid/invalid values per resource manager API
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *   NONE
 * \par INDIRECT OUTPUT
 *   the executed test
 */
static shr_error_e
dnx_algo_res_check_input_valid_values(
    int unit)
{
    int element;
    shr_error_e rv;
    uint8 is_allocated;
    uint32 core_id, sub_resource_id;
    sw_state_algo_res_create_data_t data_core_all, illegal_data, data_per_core, data_indexed_resource,
        data_indexed_resource_per_core;

    SHR_FUNC_INIT_VARS(unit);

    rv = _SHR_E_NONE;

    sal_memset(&data_core_all, 0, sizeof(sw_state_algo_res_create_data_t));
    data_core_all.flags = FLAGS_NONE;
    data_core_all.first_element = RESOURCE_FIRST_ELEMENT;
    data_core_all.nof_elements = RESOURCE_NOF_ELEMENTS;
    sal_strncpy(data_core_all.name, RESOURCE_TEST_INPUT_VALID_CORE_ALL, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    sal_memset(&illegal_data, 0, sizeof(sw_state_algo_res_create_data_t));
    illegal_data.flags = FLAGS_NONE;
    illegal_data.first_element = -1;
    illegal_data.nof_elements = RESOURCE_NOF_ELEMENTS;
    sal_strncpy(illegal_data.name, RESOURCE_TEST_INVALID, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    sal_memset(&data_per_core, 0, sizeof(sw_state_algo_res_create_data_t));
    data_per_core.flags = SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE;
    data_per_core.first_element = RESOURCE_FIRST_ELEMENT;
    data_per_core.nof_elements = RESOURCE_NOF_ELEMENTS;
    sal_strncpy(data_per_core.name, RESOURCE_TEST_INPUT_VALID_PER_CORE, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    sal_memset(&data_indexed_resource, 0, sizeof(sw_state_algo_res_create_data_t));
    data_indexed_resource.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data_indexed_resource.first_element = RESOURCE_FIRST_ELEMENT;
    data_indexed_resource.nof_elements = RESOURCE_NOF_ELEMENTS;
    data_indexed_resource.nof_resource_pool_indexes = NOF_SUB_RESOURCES;
    sal_strncpy(data_indexed_resource.name, RESOURCE_TEST_INPUT_VALID_INDEXED_RESOURCE,
                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    sal_memset(&data_indexed_resource_per_core, 0, sizeof(sw_state_algo_res_create_data_t));
    data_indexed_resource_per_core.flags =
        SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE | SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data_indexed_resource_per_core.first_element = RESOURCE_FIRST_ELEMENT;
    data_indexed_resource_per_core.nof_elements = RESOURCE_NOF_ELEMENTS;
    data_indexed_resource_per_core.nof_resource_pool_indexes = NOF_SUB_RESOURCES;
    sal_strncpy(data_indexed_resource_per_core.name, RESOURCE_TEST_INPUT_VALID_INDEXED_RESOURCE_PER_CORE,
                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /************************************
     * check sw_state_algo_res_create inputs
     ***********************************/
    /*
     * {
     */
    /*
     * the resources which are created in this staged will be used by the rest of the APIs later
     */
    /*
     * running sw_state_algo_res_create for RESOURCE_TEST_INPUT_VALID_INDEXED_RESOURCE_PER_CORE - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.alloc
                    (unit, SW_STATE_ALGO_RES_NOF_CORES_GET(unit), NOF_SUB_RESOURCES));

    for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
    {
        for (sub_resource_id = 0; sub_resource_id < NOF_SUB_RESOURCES; sub_resource_id++)
        {
            SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.create
                            (unit, core_id, sub_resource_id, &data_indexed_resource_per_core, NULL));
        }
    }
    /*
     * running sw_state_algo_res_create for new resource name - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_core_all.create(unit, &data_core_all, NULL));
    /*
     * running sw_state_algo_res_create for SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_per_core.alloc
                    (unit, SW_STATE_ALGO_RES_NOF_CORES_GET(unit)));

    for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_per_core.create
                        (unit, core_id, &data_per_core, NULL));
    }
    /*
     * running sw_state_algo_res_create for RESOURCE_TEST_INPUT_VALID_INDEXED_RESOURCE - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource.alloc(unit, NOF_SUB_RESOURCES));
    for (sub_resource_id = 0; sub_resource_id < NOF_SUB_RESOURCES; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource.create
                        (unit, sub_resource_id, &data_indexed_resource, NULL));
    }
    /*
     * running sw_state_algo_res_create for already existing name - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_core_all.create(unit, &data_core_all, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "resource manager with the same name already exists but create was successful\n");
    }
    /*
     * running sw_state_algo_res_create with negative first element - should fail
     */
    rv = algo_res_mngr_db.resource_test_invalid.create(unit, &illegal_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "first_element is negative but sw_state_algo_res_create succeeded\n");
    }
    /*
     * running sw_state_algo_res_create with zero nof_elements - should fail
     */
    illegal_data.first_element = RESOURCE_FIRST_ELEMENT;
    illegal_data.nof_elements = ILLEGAL_NOF_ELEMENTS;
    rv = algo_res_mngr_db.resource_test_invalid.create(unit, &illegal_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "nof_elements = 0 but sw_state_algo_res_create succeeded\n");
    }

    /*
     * }
     */
    /************************************
     * check sw_state_algo_res_allocate inputs
     ***********************************/
    /*
     * {
     */
    /*
     * running sw_state_algo_res_allocate with core_id = 0 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_per_core.allocate_single
                    (unit, FIRST_CORE_ID, FLAGS_NONE, NULL, &element));
    /*
     * running sw_state_algo_res_allocate with core_id = 1 - should pass
     */
    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_per_core.allocate_single
                        (unit, FIRST_CORE_ID, FLAGS_NONE, NULL, &element));
    }
    /*
     * running sw_state_algo_res_allocate with core_id = 0 and pool_id = 0 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource.allocate_single
                    (unit, FIRST_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element));
    /*
     * running sw_state_algo_res_allocate with core_id = 0 and pool_id = 1 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource.allocate_single
                    (unit, SECOND_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element));

    /*
     * running sw_state_algo_res_allocate with core_id = 0 and and pool_id = 0 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.allocate_single
                    (unit, FIRST_CORE_ID, FIRST_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element));
    /*
     * running sw_state_algo_res_allocate with core_id = 1 and pool_id = 0 - should pass
     */
    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.allocate_single
                        (unit, SECOND_CORE_ID, FIRST_CORE_ID, FLAGS_NONE, NULL, &element));
    }
    /*
     * running sw_state_algo_res_allocate with core_id = 0 and and pool_id = 1 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.allocate_single
                    (unit, FIRST_CORE_ID, SECOND_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element));
    /*
     * running sw_state_algo_res_allocate with core_id = 1 and pool_id = 1 - should pass
     */
    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.allocate_single
                        (unit, SECOND_CORE_ID, SECOND_SUB_RESOURCE_INDX, FLAGS_NONE, NULL, &element));
    }

    /*
     * running sw_state_algo_res_allocate with negative core_id - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_per_core.allocate_single
        (unit, ILLEGAL_CORE_ID, FLAGS_NONE, NULL, &element);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "core_id is negative but allocate was successful\n");
    }
    /*
     * running sw_state_algo_res_allocate with core_id = nof_cores - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_per_core.allocate_single
        (unit, dnx_data_device.general.nof_cores_get(unit), FLAGS_NONE, NULL, &element);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "core_id = nof_cores but allocate was successful\n");
    }

    /*
     * }
     */
    /************************************
     * test sw_state_algo_res_is_allocated inputs
     ***********************************/
    /*
     * {
     */
    /*
     * running sw_state_algo_res_is_allocated with out of range element RESOURCE_NOF_ELEMENTS - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_core_all.is_allocated(unit, RESOURCE_NOF_ELEMENTS, &is_allocated);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "out of range but sw_state_algo_res_is_allocated was successful\n");
    }
    /*
     * running sw_state_algo_res_is_allocated with out of range element -1 - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_core_all.is_allocated(unit, ILLEGAL_ELEMENT_ID, &is_allocated);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "out of range but sw_state_algo_res_is_allocated was successful\n");
    }
    /*
     * running sw_state_algo_res_is_allocated with core_id = 0 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_per_core.is_allocated
                    (unit, FIRST_CORE_ID, 2, &is_allocated));
    /*
     * running sw_state_algo_res_is_allocated with core_id = 1 - should pass
     */
    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_per_core.is_allocated
                        (unit, SECOND_CORE_ID, 2, &is_allocated));
    }
    /*
     * running sw_state_algo_res_is_allocated with  pool_id = 0 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource.is_allocated
                    (unit, FIRST_SUB_RESOURCE_INDX, 2, &is_allocated));
    /*
     * running sw_state_algo_res_is_allocated with pool_id = 1 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource.is_allocated
                    (unit, SECOND_SUB_RESOURCE_INDX, 2, &is_allocated));

    /*
     * running sw_state_algo_res_is_allocated with core_id = 0 and pool_id = 0 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.is_allocated
                    (unit, FIRST_CORE_ID, FIRST_SUB_RESOURCE_INDX, 2, &is_allocated));
    /*
     * running sw_state_algo_res_is_allocated with core_id = 0 and pool_id = 1 - should pass
     */
    SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.is_allocated
                    (unit, FIRST_CORE_ID, SECOND_SUB_RESOURCE_INDX, 2, &is_allocated));
    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {
        /*
         * running sw_state_algo_res_is_allocated with core_id = 1 and pool_id = 0 - should pass
         */
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.is_allocated
                        (unit, SECOND_CORE_ID, FIRST_SUB_RESOURCE_INDX, 2, &is_allocated));
        /*
         * running sw_state_algo_res_is_allocated with core_id = 1 and pool_id = 1 - should pass
         */
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource_test_input_valid_indexed_resource_per_core.is_allocated
                        (unit, SECOND_CORE_ID, SECOND_SUB_RESOURCE_INDX, 2, &is_allocated));
    }

    /*
     * }
     */
    /************************************
     * test sw_state_algo_res_free input
     ***********************************/
    /*
     * {
     */

    /*
     * running sw_state_algo_res_free with out of range element 200 - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_core_all.free_single(unit, RESOURCE_NOF_ELEMENTS);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "out of range but sw_state_algo_res_free was successful\n");
    }
    /*
     * running sw_state_algo_res_free with out of range element -1 - should fail
     */
    rv = algo_res_mngr_db.resource_test_input_valid_core_all.free_single(unit, ILLEGAL_ELEMENT_ID);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "out of range but sw_state_algo_res_free was successful\n");
    }
    /*
     * }
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * run resource manager algorithm unit test
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_algo_res_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bsl_severity_t original_severity_sw_state, original_res_mngr;
    SHR_FUNC_INIT_VARS(unit);

    original_severity_sw_state = BSL_INFO;
    original_res_mngr = BSL_INFO;

    /*
     * use err reocvery to revert system state after test
     */
    dnx_state_comparison_start(unit);
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    /*
     * Turning on the alloc during test flag (set it to 1)
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /*
     * save the original severity level of the sw state and severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_res_mngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);

    SHR_IF_ERR_EXIT(algo_res_mngr_db.init(unit));

    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {
        SHR_IF_ERR_EXIT(dnx_algo_res_per_core_scenario(unit));
    }

    SHR_IF_ERR_EXIT(dnx_algo_res_core_all_scenario(unit));

    SHR_IF_ERR_EXIT(dnx_algo_res_check_input_valid_values(unit));

exit:
    /*
     * set back the severity level of the resource manager and sw state to theirs original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_res_mngr);

    /*
     * Turning off the alloc during test flag (set it to 0)
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));

    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

    dnx_state_comparison_end_and_compare(unit);

    SHR_FUNC_EXIT;
}
