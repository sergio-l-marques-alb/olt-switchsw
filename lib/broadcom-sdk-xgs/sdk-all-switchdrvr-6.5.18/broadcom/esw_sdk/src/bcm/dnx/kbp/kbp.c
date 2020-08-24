/** \file kbp.c
 *
 * Functions for handling all the kbp init sequences.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#if defined(INCLUDE_KBP)

#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_xpt.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <bcm/switch.h>
#include <soc/dnxc/dnxc_ha.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES] = { NULL };
kbp_warmboot_t Kbp_warmboot_data[SOC_MAX_NUM_DEVICES] = { {NULL}
};
FILE *Kbp_file_fp[SOC_MAX_NUM_DEVICES] = { NULL };

/*
 * }
 */

/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * \brief
 *   Init KBP shared info for KBP application.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_generic_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Init the global structure */

    if (Kbp_app_data[unit] == NULL)
    {
        SHR_ALLOC_SET_ZERO(Kbp_app_data[unit], sizeof(generic_kbp_app_data_t), "Kbp_app_data[unit]", "%s%d%s\r\n",
                           "unit #", unit, EMPTY);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INIT, " Error: KBP is already initialized for device.\n");
    }

    /*
     * Create the default allocator
     */
    DNX_KBP_TRY(default_allocator_create(&Kbp_app_data[unit]->dalloc_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   De-init KBP generic.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static shr_error_e
dnx_kbp_generic_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((Kbp_app_data[unit] == NULL) || (Kbp_app_data[unit]->dalloc_p == NULL))
    {
        /*
         * Nothing needs to deinit, exit directly.
         */
        SHR_EXIT();
    }

    /** Free Allocator */
    DNX_KBP_TRY(default_allocator_destroy(Kbp_app_data[unit]->dalloc_p));

    /** Free Global struct */
    SHR_FREE(Kbp_app_data[unit]);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure the KBP general data.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static shr_error_e
dnx_kbp_init_general_data_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get user data
     */
    SHR_IF_ERR_EXIT(dnx_kbp_init_user_data_get(unit, &Kbp_app_data[unit]->user_data));
    /*
     * Get device type
     */
    SHR_IF_ERR_EXIT(dnx_kbp_init_device_type_get(unit, &Kbp_app_data[unit]->device_type));
    /*
     * Get Interface config data
     */
    SHR_IF_ERR_EXIT(dnx_kbp_init_config_param_get
                    (unit, 0, &Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   KBP initialization sequences.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(dnx_kbp_generic_init(unit));

        SHR_IF_ERR_EXIT(dnx_kbp_init_general_data_config(unit));

        if (!dnx_data_elk.connectivity.feature_get(unit, dnx_data_elk_connectivity_blackhole))
        {
            if (!sw_state_is_warm_boot(unit))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_init_reset(unit));
            }
            SHR_IF_ERR_EXIT(dnx_kbp_xpt_init(unit));
        }

        if (!sw_state_is_warm_boot(unit))
        {
            if (dnx_kbp_interface_enabled(unit))
            {
                SHR_IF_ERR_EXIT(dnx_kbp_interface_init(unit));

                SHR_IF_ERR_EXIT(dnx_kbp_init_rop_test(unit));
            }
            if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_init_kbp_stat_interface(unit));
            }
        }

        SHR_IF_ERR_EXIT(dnx_kbp_device_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   KBP de-initialization sequences.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
        SHR_IF_ERR_EXIT(dnx_kbp_device_deinit(unit));

        if (dnx_kbp_interface_enabled(unit))
        {
            SHR_IF_ERR_EXIT(dnx_kbp_xpt_deinit(unit));
        }
        SHR_IF_ERR_EXIT(dnx_kbp_generic_deinit(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   KBP device pointer get.
 * \param [in] unit - The unit number.
 * \param [out] device_p - kbp device pointer
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_info_get(
    int unit,
    bcm_switch_kbp_info_t * kbp_info)
{
    int thread_index;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_kbp_device_enabled(unit))
    {
        kbp_info->device_p = (int *) Kbp_app_data[unit]->device_p;
        for (thread_index = 0; thread_index < BCM_KBP_MAX_NOF_THREADS; thread_index++)
        {
            kbp_info->thread_p[thread_index] = (int *) Kbp_app_data[unit]->smt_p[thread_index];
        }
    }
    else
    {
        kbp_info->device_p = NULL;
        for (thread_index = 0; thread_index < BCM_KBP_MAX_NOF_THREADS; thread_index++)
        {
            kbp_info->thread_p[thread_index] = NULL;
        }
    }
/**exit:*/
    SHR_FUNC_EXIT;
}

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */

#undef BSL_LOG_MODULE
