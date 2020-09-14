/* \file diag_dnx_multicast_tests.c
 *
 * Tests for Multicast
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/************
 * INCLUDES  *
 *************/
#include "ctest_dnx_multicast_tests.h"
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <src/bcm/dnx/multicast/multicast_imp.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm/mcast.h>
#include <bcm_int/dnx_dispatch.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <sal/core/libc.h>
#include <shared/utilex/utilex_os_interface.h>

/************
 * TYPEDEFS  *
 *************/

typedef struct
{
    int nof_replications;
    unsigned nof_groups;
    int is_ingress;
} test_iter_info;

 /************
 *  DEFINES   *
 *************/

#define DNX_MULTICAST_MAX_CUD            (DNX_MULTICAST_INVALID_CUD - 1)
#define DNX_BMP_MC_ID_GET(group, flags)  DNX_MULTICAST_IS_EGRESS(flags) ? _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_EGR_BMP, group) : _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_ING_BMP, group)

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

/************
* FUNCTIONS *
*************/

/** Compare function for bcm_multicast_replication_t */
static int
bcm_multicast_replication_t_compare(
    void *a,
    void *b)
{
    const bcm_multicast_replication_t *ca = b;
    const bcm_multicast_replication_t *cb = a;

    if (ca->port > cb->port)
    {
        return 1;
    }
    else if (ca->port < cb->port)
    {
        return -1;
    }
    else
    {
        if (ca->encap1 > cb->encap1)
        {
            return 1;
        }
        else if (ca->encap1 < cb->encap1)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

/** Function to randomize the multicast IDs array */
static void
test_mc_ids_randomize(
    int unit,
    bcm_multicast_t * mc_ids,
    uint32 nof_mc_ids,
    uint32 seed)
{
    uint32 mc_id_index, rand_mc_id_index, temp;

    LOG_CLI((BSL_META("Ramdomizing MC IDs array with seed=%d \n"), seed));

    for (mc_id_index = 0; mc_id_index < nof_mc_ids; ++mc_id_index)
    {
        rand_mc_id_index = sal_rand() % nof_mc_ids;
        temp = mc_ids[mc_id_index];
        mc_ids[mc_id_index] = mc_ids[rand_mc_id_index];
        mc_ids[rand_mc_id_index] = temp;
    }
}

/** Function to populate the multicast IDs array */
static shr_error_e
test_mc_ids_populate(
    int unit,
    bcm_multicast_t * mc_ids,
    uint32 nof_mc_ids)
{
    shr_error_e rv;
    uint32 mc_id_index;

    SHR_FUNC_INIT_VARS(unit);

    rv = sh_process_command(unit, "debug bcmdnx multicast -");
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - BCM Shell command failed!");
    }
    for (mc_id_index = 0; mc_id_index < nof_mc_ids; ++mc_id_index)
    {
        /** Destroy in case some group exists */
        bcm_dnx_multicast_destroy(unit, mc_id_index);

        mc_ids[mc_id_index] = mc_id_index;
    }

exit:
    rv = sh_process_command(unit, "debug bcmdnx multicast =");
    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("Fail - BCM Shell command failed!")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
    SHR_FUNC_EXIT;
}

/** Function to sort and compare replication arrays */
static shr_error_e
test_mc_repl_compare(
    int unit,
    bcm_multicast_replication_t * repl_in,
    bcm_multicast_replication_t * repl_out,
    uint32 nof_repl)
{
    uint32 repl_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sort replication arrays in order to compare
     */
    utilex_os_qsort(repl_in, nof_repl, sizeof(bcm_multicast_replication_t), bcm_multicast_replication_t_compare);
    utilex_os_qsort(repl_out, nof_repl, sizeof(bcm_multicast_replication_t), bcm_multicast_replication_t_compare);

    /*
     * Compare replication arrays
     */
    for (repl_index = 0; repl_index < nof_repl; repl_index++)
    {
        if (!(repl_in[repl_index].port == repl_out[repl_index].port))
        {
            if ((repl_in[repl_index].encap1 != repl_out[repl_index].encap1)
                && repl_out[repl_index].encap1 != BCM_IF_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - bcm_multicast_get output mismatch for replications");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create multicast groups in random order and verify that they were siccessfully created
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] flags - flags for the multicast groups
 *   \param [in] mc_ids - array with multicast IDs to be created
 *   \param [in] nof_mc_ids - number of multicast IDs to be created
 *   \param [in] seed - Test seed
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
test_mc_create(
    int unit,
    uint32 flags,
    bcm_multicast_t * mc_ids,
    uint32 nof_mc_ids,
    uint32 seed)
{
    bcm_multicast_replication_t repl_out;
    uint32 mc_id_index;
    int count;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Randomize group ID selection array
     */
    test_mc_ids_randomize(unit, mc_ids, nof_mc_ids, seed);

    /*
     * For each group in random order - create, add/set, get (to verify)
     */
    for (mc_id_index = 0; mc_id_index < nof_mc_ids; ++mc_id_index)
    {
        /*
         * Create the multicast group
         */
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_create(unit, flags, &mc_ids[mc_id_index]));

        /*
         * Verify that the group is opened and empty
         */
        count = 0;
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_get(unit, mc_ids[mc_id_index], flags, 1, &repl_out, &count));
        if (count != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Fail - bcm_multicast_get output mismatch for group %d, group not empty", mc_ids[mc_id_index]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ot add replications to multicast groups in random order and with random CUD, verify that they were siccessfully added/set
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] flags - flags for the multicast groups
 *   \param [in] gport - array gports for the replications
 *   \param [in] gport_size - number of mrmbers in the gport array
 *   \param [in] nof_repl - number of replications to be set, can be bigger that the gport_size. In that case some of the gports will be rrepeated with different CUDs
 *   \param [in] mc_ids - array with multicast IDs
 *   \param [in] nof_mc_ids - number of multicast IDs
 *   \param [in] seed - Test seed
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
test_mc_set_add(
    int unit,
    uint32 flags,
    bcm_gport_t * gport,
    int gport_size,
    int nof_repl,
    bcm_multicast_t * mc_ids,
    uint32 nof_mc_ids,
    uint32 seed)
{
    shr_error_e rv = _SHR_E_NONE;
    bcm_multicast_replication_t *repl_in = NULL;
    bcm_multicast_replication_t *repl_out = NULL;
    uint32 mc_id_index, repl_index, cud;
    uint8 set_or_add;
    int count, nof_remaining, nof_repl_add = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocations
     */
    SHR_ALLOC(repl_in, sizeof(bcm_multicast_replication_t) * (nof_repl), "repl_in", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    sal_memset(repl_in, 0, sizeof(bcm_multicast_replication_t) * (nof_repl));
    SHR_ALLOC(repl_out, sizeof(bcm_multicast_replication_t) * (nof_repl), "repl_out", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);
    sal_memset(repl_out, 0, sizeof(bcm_multicast_replication_t) * (nof_repl));

    /*
     * Randomize group ID selection array
     */
    test_mc_ids_randomize(unit, mc_ids, nof_mc_ids, seed);

    /*
     * For each group in random order - create, add/set, get (to verify)
     */
    for (mc_id_index = 0; mc_id_index < nof_mc_ids; ++mc_id_index)
    {
        /*
         * CUD for the bitmap groups, needs to be the same for all replications
         */
        cud = mc_id_index % DNX_MULTICAST_MAX_CUD;

        /*
         * Create the replication array
         */
        for (repl_index = 0; repl_index < nof_repl; repl_index++)
        {
            /*
             * If nof_repl is more than the number of Gports in the Gports array, fill the extra replications with the same Gports
             */
            repl_in[repl_index].port = gport[repl_index % gport_size];

            if (DNX_MULTICAST_IS_BITMAP(flags))
            {
                repl_in[repl_index].encap1 = cud;
            }
            else
            {
                /*
                 * random CUD for each replication in the Linked list groups
                 */
                cud = mc_id_index % DNX_MULTICAST_MAX_CUD;
                repl_in[repl_index].encap1 = cud;
            }
        }

        /*
         * randomly choose which API to test (set or add)
         */
        set_or_add = mc_id_index % 2;
        if (set_or_add != 0)
        {
            /*
             * Add 3 replications each iteration, until all the replications are added
             */
            for (repl_index = 0; repl_index < nof_repl; repl_index += 3)
            {
                nof_repl_add = (nof_repl - repl_index < 3) ? (nof_repl - repl_index) : 3;
                rv = bcm_dnx_multicast_add(unit, mc_ids[mc_id_index], flags, nof_repl_add, (repl_in + repl_index));
                if (rv != _SHR_E_NONE)
                {
                    break;
                }
            }
        }
        else
        {
            rv = bcm_dnx_multicast_set(unit, mc_ids[mc_id_index], flags, nof_repl, repl_in);
        }

        if (rv == _SHR_E_NONE)
        {
            /*
             * Get group
             */
            count = 0;
            SHR_IF_ERR_EXIT(bcm_dnx_multicast_get(unit, mc_ids[mc_id_index], flags, nof_repl, repl_out, &count));
            /*
             * Verify replication count
             */
            if (nof_repl != count)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Fail - bcm_multicast_get output mismatch for group %d, nof_repl=%d count=%d, flags=%d",
                             mc_ids[mc_id_index], nof_repl, count, flags);
            }

            /*
             * Compare replications
             */
            SHR_IF_ERR_EXIT(test_mc_repl_compare(unit, repl_in, repl_out, nof_repl));

        }
        else if (rv == _SHR_E_FULL)
        {
            /*
             * For the Full-Delete test, to make sure all the resources are filled
             */
            SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_remaining));
            if (nof_remaining != 0)
            {
                SHR_IF_ERR_EXIT(bcm_dnx_multicast_set(unit, mc_ids[mc_id_index], flags, nof_remaining, repl_in));
                /*
                 * Get group
                 */
                count = 0;
                SHR_IF_ERR_EXIT(bcm_dnx_multicast_get(unit, mc_ids[mc_id_index], flags, nof_remaining,
                                                      repl_out, &count));
                /*
                 * Verify replication count
                 */
                if (nof_remaining != count)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Fail - bcm_multicast_get output mismatch for group %d, nof_repl=%d count=%d, flags=%d",
                                 mc_ids[mc_id_index], nof_remaining, count, flags);
                }

                /*
                 * Compare replications
                 */
                SHR_IF_ERR_EXIT(test_mc_repl_compare(unit, repl_in, repl_out, nof_remaining));
            }
            SHR_SET_CURRENT_ERR(_SHR_E_FULL);
            break;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FREE(repl_in);
    SHR_FREE(repl_out);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete replications from multicast groups in random order and verify that they were siccessfully deleted
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] flags - flags for the multicast groups
 *   \param [in] nof_repl - number of replications to be set.
 *   \param [in] mc_ids - array with multicast IDs
 *   \param [in] nof_mc_ids - number of multicast IDs
 *   \param [in] seed - Test seed
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
test_mc_delete_all(
    int unit,
    uint32 flags,
    int nof_repl,
    bcm_multicast_t * mc_ids,
    uint32 nof_mc_ids,
    uint32 seed)
{
    bcm_multicast_replication_t repl_in;
    bcm_multicast_replication_t *repl_out = NULL;
    uint32 mc_id_index;
    int count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(repl_out, sizeof(bcm_multicast_replication_t) * (nof_repl), "repl_out", "%s%s%s\r\n", EMPTY, EMPTY,
              EMPTY);
    sal_memset(repl_out, 0, sizeof(bcm_multicast_replication_t) * (nof_repl));

    /*
     * Randomize group ID selection array
     */
    test_mc_ids_randomize(unit, mc_ids, nof_mc_ids, seed);

    /*
     * For each group in random order - delete replications, get (to verify), destroy
     */
    for (mc_id_index = 0; mc_id_index < nof_mc_ids; ++mc_id_index)
    {
        /*
         * Delete the replications
         */
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_delete
                        (unit, mc_ids[mc_id_index], (flags | BCM_MULTICAST_REMOVE_ALL), 1, &repl_in));

        /*
         * Get group to verify the replications were deleted
         */
        count = 0;
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_get(unit, mc_ids[mc_id_index], flags, nof_repl, repl_out, &count));
        if (count != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Fail - bcm_multicast_get output mismatch for group %d. Expected 0, but received %d members.",
                         _BCM_MULTICAST_ID_GET(mc_ids[mc_id_index]), count);
        }
    }

exit:
    SHR_FREE(repl_out);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete multicast groups in random order and verify that they were siccessfully deleted
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] flags - flags for the multicast groups
 *   \param [in] mc_ids - array with multicast IDs to be deleted
 *   \param [in] nof_mc_ids - number of multicast IDs to be deleted
 *   \param [in] seed - Test seed
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
test_mc_destroy(
    int unit,
    uint32 flags,
    bcm_multicast_t * mc_ids,
    uint32 nof_mc_ids,
    uint32 seed)
{
    shr_error_e rv;
    bcm_multicast_replication_t repl_in;
    uint32 mc_id_index;
    int count;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Randomize group ID selection array
     */
    test_mc_ids_randomize(unit, mc_ids, nof_mc_ids, seed);

    rv = sh_process_command(unit, "debug bcmdnx multicast -");
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - BCM Shell command failed!");
    }
    /*
     * For each group in random order - delete replications, get (to verify), destroy
     */
    for (mc_id_index = 0; mc_id_index < nof_mc_ids; ++mc_id_index)
    {
        /*
         * Destroy the group
         */
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_destroy(unit, mc_ids[mc_id_index]));

        /*
         * Check if group was destroyed
         */
        rv = bcm_dnx_multicast_get(unit, mc_ids[mc_id_index], flags, 1, &repl_in, &count);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Group %d was not destroyed!", mc_ids[mc_id_index]);
        }
    }

exit:
    rv = sh_process_command(unit, "debug bcmdnx multicast =");
    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("Fail - BCM Shell command failed!")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the MC data base by creating all Egress linked List groups and adding one replication to each.
 *          The replications are then deleted and the groups are destroyed. Resources are checked in the start and end.
 *          The process is repeated for Egress Bitmap, Ingress Linked List and Ingress Bitmap.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_ing - indicates if ingress or egress MC groups will be tested.
 * \param [in] seed - Test seed
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
sh_dnx_one_replication_per_group(
    int unit,
    int is_ing,
    uint32 seed)
{
    bcm_port_t port = 0;
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    bcm_gport_t gport = 0;
    int nof_replications = 1;
    int nof_free_start, nof_free_end, port_index = 0;
    uint32 nof_mc_ids = 0;
    bcm_multicast_t *mc_ids = NULL;
    uint32 iter, flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        /*
         * Take 1 ports for the test
         */
        if (port_index >= 1)
        {
            break;
        }

        port = logical_port;
        port_index++;
    }

    /*
     * Four iterations for each type MC group: Egress Linked list and Bmp; Ingress Linked Lis and Bmp
     */
    for (iter = 0; iter < 4; iter++)
    {

        if ((is_ing == TRUE) && (iter <= 1))
        {
            continue;
        }
        else if ((is_ing == FALSE) && (iter >= 2))
        {
            continue;
        }

        switch (iter)
        {
            case 0:
                /*
                 * Egress Linked list
                 */
                flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP);
                nof_mc_ids = dnx_data_multicast.params.max_egr_mc_groups_get(unit);
                BCM_GPORT_LOCAL_SET(gport, port);
                LOG_CLI((BSL_META("Starting iteration %d Egress Linked list groups... \n"), iter));
                break;
            case 1:
                /*
                 * Egress Bitmap
                 */
                flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_PORTS_GROUP);
                nof_mc_ids = dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit);
                BCM_GPORT_LOCAL_SET(gport, port);
                LOG_CLI((BSL_META("Starting iteration %d Egress Bitmap groups... \n"), iter));
                break;
            case 2:
                /*
                 * Ingress Linked list
                 */
                flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);
                nof_mc_ids = dnx_data_multicast.params.max_ing_mc_groups_get(unit);
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
                LOG_CLI((BSL_META("Starting iteration %d Ingress Linked list groups... \n"), iter));
                break;
            case 3:
                /*
                 * Ingress Bitmap
                 */
                flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_PORTS_GROUP);
                nof_mc_ids = dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit);
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
                LOG_CLI((BSL_META("Starting iteration %d Egress Bitmap groups... \n"), iter));
                break;
        }

        SHR_ALLOC(mc_ids, sizeof(bcm_multicast_t) * (nof_mc_ids), "mc_ids", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /*
         * Populate the group ID array
         */
        SHR_IF_ERR_EXIT(test_mc_ids_populate(unit, mc_ids, nof_mc_ids));

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_start));
        LOG_CLI((BSL_META("Check how many free entries at MCDB %d... \n"), nof_free_start));

        /*
         * Create all groups and set/add one replication to groups
         */
        LOG_CLI((BSL_META("Create all groups for iteration %d and set/add one replication... \n"), iter));
        SHR_IF_ERR_EXIT(test_mc_create(unit, flags, mc_ids, nof_mc_ids, seed));
        SHR_IF_ERR_EXIT(test_mc_set_add(unit, flags, &gport, 1, nof_replications, mc_ids, nof_mc_ids, seed));
        LOG_CLI((BSL_META("Done. \n")));

        /*
         * Delete all replications and destroy all groups
         */
        LOG_CLI((BSL_META("Delete all replications and destroy all groups for iteration %d... \n"), iter));
        SHR_IF_ERR_EXIT(test_mc_delete_all(unit, flags, nof_replications, mc_ids, nof_mc_ids, seed));
        SHR_IF_ERR_EXIT(test_mc_destroy(unit, flags, mc_ids, nof_mc_ids, seed));
        LOG_CLI((BSL_META("Done. \n")));
        SHR_FREE(mc_ids);
        mc_ids = NULL;

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_end));

        if (nof_free_end != nof_free_start)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - resources were not freed! nof_free_end=%d nof_free_start=%d \n",
                         nof_free_end, nof_free_start);
        }
    }

exit:
    if (mc_ids != NULL)
    {
        SHR_FREE(mc_ids);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the MC data base by adding and removing a number of ingress and egress groups with many replications. 
 *          The groups are added and deleted in random order and the resources are checked after each stage.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] seed - Test seed
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
sh_dnx_many_replications_per_group(
    int unit,
    uint32 seed)
{
    bcm_port_t port[4];
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    bcm_gport_t gport[6];
    int nof_replications, port_index = 0;
    int nof_free_start, nof_free_end;
    uint32 flags, iter_size;
    int gport_index, iter_index, nof_mc_ids;
    bcm_multicast_t *mc_ids = NULL;
    bcm_multicast_t grp_test_ll, grp_test_bmp;
    test_iter_info iter[] = {
          /**Reps  Groups  Is_Ing  Is_Bmp*/
        {6, 1538, 0},             /**Egr*/
        {1, 701, 1},              /**Ingr*/
        {17, 279, 0},             /**Egr*/
        {5, 311, 1},              /**Ingr*/
        {801, 5, 0},              /**Egr*/
        {128, 19, 1},             /**Ingr*/
        {132, 17, 0},             /**Egr*/
        {4094, 3, 1},             /**Ingr*/
        {1, 1474, 0},             /**Egr*/
        {17, 203, 1},             /**Ingr*/
        {2311, 4, 0},             /**Egr*/
        {1325, 7, 1},             /**Ingr*/
        {4095, 2, 0},             /**Egr*/
        {2, 1429, 1}
    };                             /**Ingr*/

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        /*
         * Take 4 ports for the test
         */
        if (port_index >= 4)
        {
            break;
        }

        port[port_index] = logical_port;
        port_index++;
    }

    /*
     * Create two test groups t fill in the last two spots of the Gports for the replication array
     */
    grp_test_ll = dnx_data_multicast.params.max_egr_mc_groups_get(unit) - 1;
    grp_test_bmp = dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit) - 1;
    SHR_IF_ERR_EXIT(bcm_dnx_multicast_create(unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP), &grp_test_ll));
    SHR_IF_ERR_EXIT(bcm_dnx_multicast_create
                    (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_PORTS_GROUP),
                     &grp_test_bmp));
    BCM_MCAST_GPORT_GROUP_ID_SET(gport[4], grp_test_ll);
    BCM_MCAST_GPORT_BITMAP_ID_SET(gport[5], grp_test_bmp);

    iter_size = sizeof(iter) / sizeof(iter[0]);
    for (iter_index = 0; iter_index < iter_size; iter_index++)
    {
        nof_replications = iter[iter_index].nof_replications;
        nof_mc_ids = iter[iter_index].nof_groups;
        flags = BCM_MULTICAST_WITH_ID;
        if (iter[iter_index].is_ingress)
        {
            flags |= BCM_MULTICAST_INGRESS_GROUP;
            for (gport_index = 0; gport_index < 4; gport_index++)
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport[gport_index], port[gport_index]);
            }
        }
        else
        {
            flags |= BCM_MULTICAST_EGRESS_GROUP;
            for (gport_index = 0; gport_index < 4; gport_index++)
            {
                BCM_GPORT_LOCAL_SET(gport[gport_index], port[gport_index]);
            }
        }

        /*
         * Init group ID selection array
         */
        SHR_ALLOC(mc_ids, sizeof(bcm_multicast_t) * (nof_mc_ids), "mc_ids", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /*
         * Populate the group ID array
         */
        SHR_IF_ERR_EXIT(test_mc_ids_populate(unit, mc_ids, nof_mc_ids));

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_start));
        LOG_CLI((BSL_META("Check how many free entries at MCDB %d... \n"), nof_free_start));

        /*
         * Create all Linked List and set/add one replication to groups
         */
        LOG_CLI((BSL_META("Create and set/add replications to groups for iterration %d \n"), iter_index));
        SHR_IF_ERR_EXIT(test_mc_create(unit, flags, mc_ids, nof_mc_ids, seed));
        SHR_IF_ERR_EXIT(test_mc_set_add(unit, flags, gport, 6, nof_replications, mc_ids, nof_mc_ids, seed));
        LOG_CLI((BSL_META("Done. \n")));

        /*
         * Delete all replications and destroy groups
         */
        LOG_CLI((BSL_META("Delete replications and destroy groups for iterration %d \n"), iter_index));
        SHR_IF_ERR_EXIT(test_mc_delete_all(unit, flags, nof_replications, mc_ids, nof_mc_ids, seed));
        SHR_IF_ERR_EXIT(test_mc_destroy(unit, flags, mc_ids, nof_mc_ids, seed));
        LOG_CLI((BSL_META("Done. \n")));
        SHR_FREE(mc_ids);
        mc_ids = NULL;

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_end));
        /*
         * ports in default mc will be deleted in the test
         */
        if (nof_free_end != nof_free_start)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - resources were not freed! nof_free_end=%d nof_free_start=%d \n",
                         nof_free_end, nof_free_start);
        }
    }

    /*
     * Destroy test groups
     */
    SHR_IF_ERR_EXIT(bcm_dnx_multicast_destroy(unit, grp_test_ll));
    SHR_IF_ERR_EXIT(bcm_dnx_multicast_destroy(unit, grp_test_bmp));

exit:
    if (mc_ids != NULL)
    {
        SHR_FREE(mc_ids);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function tests the MC data base feature in a very basic 
 *          way: It fills the all entries in the table with Ingress groups in random order and replications, then it deletes all the replications and destroys the groups.
 *          The resources are checked in the start and ent to make sure there is no loss. The same process is then repeated with Egress groups.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_ing - indicates if ingress or egress MC groups will be tested.
 * \param [in] seed - Test seed
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
sh_dnx_full_table_delete(
    int unit,
    int is_ing,
    uint32 seed)
{
    shr_error_e rv = _SHR_E_NONE;
    bcm_port_t port[4];
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    bcm_gport_t gport[4];
    int nof_free_start, nof_free_end, nof_replications = 9;
    int gport_index, nof_mc_ids, port_index = 0;
    bcm_multicast_t *mc_ids = NULL;
    uint32 test_ing, test_egr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_ports));
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        /*
         * Take 4 ports for the test
         */
        if (port_index >= 4)
        {
            break;
        }

        port[port_index] = logical_port;
        port_index++;
    }

    if (is_ing == -1)
    {
        test_ing = TRUE;
        test_egr = TRUE;
    }
    else
    {
        test_ing = is_ing;
        test_egr = !(is_ing);
    }

    /*
     * Allocate MCIDs array
     */
    nof_mc_ids =
        MAX(dnx_data_multicast.params.max_ing_mc_groups_get(unit),
            dnx_data_multicast.params.max_egr_mc_groups_get(unit));
    SHR_ALLOC(mc_ids, sizeof(bcm_multicast_t) * (nof_mc_ids), "mc_ids", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (test_ing == TRUE)
    {
        for (gport_index = 0; gport_index < port_index; gport_index++)
        {
            BCM_GPORT_SYSTEM_PORT_ID_SET(gport[gport_index], port[gport_index]);
        }

        /*
         * Init group ID selection array for Ingress Linked List
         */
        nof_mc_ids = dnx_data_multicast.params.max_ing_mc_groups_get(unit);

        /*
         * Populate the group ID array
         */
        SHR_IF_ERR_EXIT(test_mc_ids_populate(unit, mc_ids, nof_mc_ids));

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_start));
        LOG_CLI((BSL_META("Check how many free entries at MCDB %d... \n"), nof_free_start));

        /*
         * Create all Ingress Linked List and set/add one replication to groups
         */
        LOG_CLI((BSL_META
                 ("Create all Ingress Linked List and set/add replications to groups until MCDB is full... \n")));
        SHR_IF_ERR_EXIT(test_mc_create
                        (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP), mc_ids, nof_mc_ids, seed));
        rv = test_mc_set_add(unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP), gport, 4, nof_replications,
                             mc_ids, nof_mc_ids, seed);
        if (rv != _SHR_E_FULL)
        {
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - MCDB was not competely filled!");
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }

        /*
         * Delete all replications and destroy all Ingress Linked List groups
         */
        LOG_CLI((BSL_META("Delete all replications and destroy all Ingress Linked List groups... \n")));
        SHR_IF_ERR_EXIT(test_mc_delete_all
                        (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP), nof_replications, mc_ids,
                         nof_mc_ids, seed));
        SHR_IF_ERR_EXIT(test_mc_destroy
                        (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP), mc_ids, nof_mc_ids, seed));
        LOG_CLI((BSL_META("Done. \n")));

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_end));
        if (nof_free_end != nof_free_start)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - resources were not entirely freed!");
        }
    }

    if (test_egr == TRUE)
    {
        for (gport_index = 0; gport_index < port_index; gport_index++)
        {
            BCM_GPORT_LOCAL_SET(gport[gport_index], port[gport_index]);
        }

        /*
         * Init group ID selection array for Egress Linked List
         */
        nof_mc_ids = dnx_data_multicast.params.max_egr_mc_groups_get(unit);

        /*
         * Populate the group ID array
         */
        SHR_IF_ERR_EXIT(test_mc_ids_populate(unit, mc_ids, nof_mc_ids));

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_start));
        LOG_CLI((BSL_META("Check how many free entries at MCDB %d... \n"), nof_free_start));

        /*
         * Create all Egress Linked List and set/add one replication to groups
         */
        LOG_CLI((BSL_META("Create all Egress Linked List and set/add replication to groups until MCDB is full... \n")));
        SHR_IF_ERR_EXIT(test_mc_create
                        (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP), mc_ids, nof_mc_ids, seed));
        rv = test_mc_set_add(unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP), gport, 4, nof_replications,
                             mc_ids, nof_mc_ids, seed);
        if (rv != _SHR_E_FULL)
        {
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - MCDB was not competely filled!");
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }

        /*
         * Delete all replications and destroy all Egress Linked List groups
         */
        LOG_CLI((BSL_META("Delete all replications and destroy all Egress Linked List groups... \n")));
        SHR_IF_ERR_EXIT(test_mc_delete_all
                        (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP), nof_replications, mc_ids,
                         nof_mc_ids, seed));
        SHR_IF_ERR_EXIT(test_mc_destroy
                        (unit, (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP), mc_ids, nof_mc_ids, seed));
        LOG_CLI((BSL_META("Done. \n")));

        /*
         * Check how many free entries at MCDB
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_end));

        if (nof_free_end != nof_free_start)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - resources were not freed! nof_free_end=%d nof_free_start=%d \n",
                         nof_free_end, nof_free_start);
        }
    }

exit:
    SHR_FREE(mc_ids);
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_tm_multicast_man = {
    "Tests for Multicast usage",
    "For test=single/multiple - Test MC data base by adding all the ingress and egress groups, each with a single/multiple replication. The groups are added and deleted in random order.\n"
        "For test=full - Test MC data base by adding ingress groups with many replications in a way that the table fully occupied. Then delete all groups and check if any resources were lost. Repeat the same for Egress groups."
        "For test=single/full - the option to specify the test mode exist. Test mode can be Ingress or Egress.\n",
    "ctest tm multicast test=<single | multiple | full> test_mode=<ing | egr | all>",
    "test=full test_mode=ing"
};

/**
 * \brief
 *   Options list for 'multicast' shell command
 */
/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_tm_multicast_options[] = {
    /* Name  Type                 Description  Default */
    {"test", SAL_FIELD_TYPE_STR, "Type of test", NULL},
    {"test_mode", SAL_FIELD_TYPE_STR, "Type of MC groups to be tested", "all"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */

/**
 * \brief
 *   List of tests for 'multicast' shell command (to be run on
 *   regression, precommit, etc.)
 */
sh_sand_invoke_t sh_dnx_tm_multicast_tests[] = {
    {"multicast_ing_single_replication", "test=single test_mode=ing", CTEST_POSTCOMMIT}
    ,
    {"multicast_multiple_replications", "test=multiple", CTEST_POSTCOMMIT}
    ,
    {"multicast_ing_full_delete", "test=full test_mode=ing", CTEST_POSTCOMMIT}
    ,
    {"multicast_egr_single_replication", "test=single test_mode=egr", CTEST_POSTCOMMIT}
    ,
    {"multicast_egr_full_delete", "test=full test_mode=egr", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
 * \brief - leaf callback for 'multicast' tests
 */
shr_error_e
sh_dnx_tm_multicast_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#if !defined(SKIP_MULTICAST_CTEST_IN_ADAPTER_SERVER_MODE)
    shr_error_e rv = _SHR_E_NONE;
#endif
    char *test_type_name = NULL;
    char *mc_type_name = NULL;
    int is_ing = 0;
    uint32 seed = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

#if defined(SKIP_MULTICAST_CTEST_IN_ADAPTER_SERVER_MODE)
    SHR_ERR_EXIT(_SHR_E_NONE, "MCDB tests are not available over adapter.\n");
#endif

    /*
     * Get the input from Shell
     */
    SH_SAND_GET_STR("test", test_type_name);
    SH_SAND_GET_STR("test_mode", mc_type_name);
    SH_SAND_GET_UINT32("seed", seed);

    if (sal_strcasecmp(mc_type_name, "ing") == 0)
    {
        is_ing = TRUE;
    }
    else if (sal_strcasecmp(mc_type_name, "egr") == 0)
    {
        is_ing = FALSE;
    }
    else if (sal_strcasecmp(mc_type_name, "all") == 0)
    {
        is_ing = -1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported MC group type! \n");
    }

    if (sal_strcasecmp(test_type_name, "single") == 0)
    {
        LOG_CLI((BSL_META("Test MC data base - adding all the groups, with a single replication.... \n")));
        SHR_IF_ERR_EXIT(sh_dnx_one_replication_per_group(unit, is_ing, seed));
    }
    else if (sal_strcasecmp(test_type_name, "multiple") == 0)
    {
        if (sal_strcasecmp(mc_type_name, "all") != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported MC group type for test 'multiple'! \n");
        }

        LOG_CLI((BSL_META
                 ("Test MC data base - adding all the ingress and egress groups, with multiple replications.... \n")));
        SHR_IF_ERR_EXIT(sh_dnx_many_replications_per_group(unit, seed));
    }
    else if (sal_strcasecmp(test_type_name, "full") == 0)
    {
        LOG_CLI((BSL_META
                 ("Test MC data base - adding groups with multiple replications until the table is completely full, then delete them.... \n")));
        SHR_IF_ERR_EXIT(sh_dnx_full_table_delete(unit, is_ing, seed));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported test type! \n");
    }

exit:
#if !defined(SKIP_MULTICAST_CTEST_IN_ADAPTER_SERVER_MODE)
    rv = ctest_dnxc_init_deinit(unit, NULL);
    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("Fail - BCM Shell command failed!")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
    }
#endif
    SHR_FUNC_EXIT;
}
