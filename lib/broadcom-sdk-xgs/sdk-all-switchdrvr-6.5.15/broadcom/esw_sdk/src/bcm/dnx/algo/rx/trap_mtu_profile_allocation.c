/*
 *  trap_mtu_profile_allocation.c
 *
 *  Created on: Jan 3, 2018
 *      Author: dp889757
 *
 *
 *      We create a custom allocation function for the MTU profile template manager.
 *
 *      The profiles should be allocated based on compressed forwarding layer type.
 *      There are 8 layer types, and we have 64 profiles, which means we have 8 profiles per layer type.
 *
 *      We allocate the profile in the corresponding block range for the supplied forwarding layer type.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set_template.h>
#include <bcm_int/dnx/algo/rx/trap_mtu_profile_allocation.h>
#include <bcm/rx.h>
#include <bcm_int/dnx/rx/rx.h>
#include <bcm_int/dnx/algo/rx/algo_rx.h>

shr_error_e
dnx_algo_rx_trap_mtu_profile_allocate(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    int tmp_profile = 0, rv, range_index, nof_profiles_per_type, prof = 1;
    int temp_nof_references = 0;
    uint8 allocation_needed = TRUE;
    dnx_rx_trap_mtu_profile_config_t *mtu_profile_config = (dnx_rx_trap_mtu_profile_config_t *) profile_data;
    dnx_rx_trap_mtu_profile_config_t get_mtu_profile_config;
    SHR_FUNC_INIT_VARS(unit);

    nof_profiles_per_type = dnx_data_trap.etpp.nof_mtu_profiles_per_fwd_type_get(unit);
    if (mtu_profile_config->is_profile_2b == TRUE)
    {
        /*
         * If it is a 2 bit MTU profile, we allocate  from profile 3 and go down to profile 1
         * for the supplied Compressed forward layer type.
         */
        prof = nof_profiles_per_type - 5;
    }
    else
    {
        /*
         * If it is a 3 bit MTU profile, we allocate  from profile 7 and go down to profile 1
         * for the supplied Compressed forward layer type.
         */
        prof = nof_profiles_per_type - 1;
    }

    /*
     * 1. Check if the data already exists or if it needs to be allocated.
     */
    rv = multi_set_template_profile_get(unit, module_id, multi_set_template, profile_data, &tmp_profile);

    /*
     * If E_NOT_FOUND was returned, it means the data wasn't allocated yet and we need to allocate it.
     */
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);

        allocation_needed = FALSE;
        *profile = tmp_profile;
    }
    else
    {
        *profile = tmp_profile;
    }

    /*
     * 2. Get the first available index for allocation.
     */
    if (allocation_needed == TRUE)
    {
        /**
         * 'mtu_profile_config.compressed_layer_type * nof_profiles_per_type' = The first element of the correct block of profiles
         *                                                      for the given forward layer type.
         * '(mtu_profile_config.compressed_layer_type+1) * nof_profiles_per_type' = The last element of the correct block of profiles
         *                                                      for the given forward layer type.
         *
         */
        for (range_index = (DNX_RX_MTU_ENTRY_INDEX_SET(prof, mtu_profile_config->compressed_layer_type));
             range_index < dnx_data_trap.etpp.nof_mtu_profiles_get(unit); prof--)
        {
            range_index = DNX_RX_MTU_ENTRY_INDEX_SET(prof, mtu_profile_config->compressed_layer_type);

            SHR_IF_ERR_EXIT(multi_set_template_profile_data_get
                            (unit, module_id, multi_set_template, range_index, &temp_nof_references,
                             &get_mtu_profile_config));
            if (temp_nof_references == 0)
            {
                *profile = range_index;
                break;
            }

        }

        if (prof < 1)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "The profile block for the current compressed forwarding type(%d) is full !",
                         mtu_profile_config->compressed_layer_type);
        }

    }

    /*
     * 3. Use the allocated profile for the multi set template WITH_ID.
     */
    SHR_IF_ERR_EXIT(multi_set_template_allocate
                    (unit, module_id, multi_set_template, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, nof_references,
                     profile_data, NULL, profile, first_reference));

    /*
     * Sanity check: allocation_needed and first_reference should be the same.
     */
    if (allocation_needed != *first_reference)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Algorithm inconsistency. allocation_needed: %d, first_reference: %d",
                     allocation_needed, *first_reference);
    }

exit:
    SHR_FUNC_EXIT;
}
