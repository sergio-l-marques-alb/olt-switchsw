/** \file src/bcm/dnx/dram/hbmc/hbmc_monitor.c
 *
 *
 *  This file contains implementation of functions for HBMC monitoring ( Temperature etc')
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/*
 * Include files.
 * {
 */

#include <sal/appl/sal.h>
#include <sal/core/dpc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/drv.h>
#include "hbmc_monitor.h"
#include "hbmc_dbal_access.h"
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include "include/soc/dnx/drv.h"
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*
 * }
 */

/*
 * defines
 * {
 */

/*
 * }
 */

typedef struct dnx_hbmc_temp_stub_s
{
    uint32 value;
    struct dnx_hbmc_temp_stub_s *next;
} dnx_hbmc_temp_stub_t;

dnx_hbmc_temp_stub_t *dnx_hbmc_temp_stub_head[BCM_LOCAL_UNITS_MAX] = { NULL };

/** See header file */
shr_error_e
dnx_hbmc_temp_get(
    int unit,
    int interface_id,
    int *value)
{
    uint32 dram_bitmap[1] = { 0 };
    uint32 nof_hbms = dnx_data_dram.hbm.nof_hbms_get(unit);
    int hbm_iter;
    int final_temp = 0;
    SHR_FUNC_INIT_VARS(unit);

    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;

    if (!(((interface_id >= 0) && (interface_id < nof_hbms)) || (interface_id == DNX_HBMC_ITER_ALL)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid interface_id=%d\n", interface_id);
    }

    /** if stub exist, return the temperature of the stub and not the real temperature - used for debug and testing */
    if (dnx_hbmc_temp_stub_head[unit] != NULL)
    {
        dnx_hbmc_temp_stub_t *remove_stub = dnx_hbmc_temp_stub_head[unit];
        *value = dnx_hbmc_temp_stub_head[unit]->value;
        dnx_hbmc_temp_stub_head[unit] = dnx_hbmc_temp_stub_head[unit]->next;
        sal_free(remove_stub);
    }
    else if (SAL_BOOT_PLISIM)
    {
        /** if on a simulation and a stub doesn't exists, return valid temp and get out */
        *value = 30;
        SHR_EXIT();
    }
    else if (interface_id == DNX_HBMC_ITER_ALL)
    {
        SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_iter)
        {
            uint32 this_hbm_temp_read = 0;
            /** temperature check is done through JTAG IEEE 1500, the WIR should be set to TEMPERATURE,
             * can be found in JEDEC. Temp is received as an 8 bit result, (MSB == 0) indicate the validity of the result.
             * other 7 bits determine a range between 0 to 127 degrees C. */
            SHR_IF_ERR_EXIT(soc_read_jr2_hbm_temp(unit, hbm_iter, &this_hbm_temp_read));
            final_temp = UTILEX_MAX(final_temp, this_hbm_temp_read);
        }
        *value = final_temp;
    }
    else
    {
        /** temperature check is done through JTAG IEEE 1500, the WIR should be set to TEMPERATURE,
         * can be found in JEDEC. Temp is received as an 8 bit result, (MSB == 0) indicate the validity of the result.
         * other 7 bits determine a range between 0 to 127 degrees C. */
        SHR_IF_ERR_EXIT(soc_read_jr2_hbm_temp(unit, interface_id, (uint32 *) value));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_hbmc_traffic_to_hbm_restore(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_restore(unit));

exit:
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
dnx_hbmc_traffic_is_empty_verify(
    int unit,
    uint8 *hbm_is_empty)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(hbm_is_empty, _SHR_E_PARAM, "hbm_is_empty");

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_hbmc_traffic_to_hbm_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_stop(unit));

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_hbmc_traffic_is_empty_get(
    int unit,
    uint8 *hbm_is_empty)
{
    uint32 nof_deleted_buffers;
    uint32 nof_buffers;
    uint32 nof_free_buffers;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_hbmc_traffic_is_empty_verify(unit, hbm_is_empty));

    /** empty quarantine FIFO */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_flush_all(unit));

    /** get how many buffers were already deleted */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_deleted_buffers_get(unit, &nof_deleted_buffers));

    /** get number of free buffers */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_free_buffers_get(unit, &nof_free_buffers));

    /** check that all buffers were freed and set indication */
    nof_buffers = nof_cores * dnx_data_dram.buffers.nof_bdbs_get(unit);
    if (nof_buffers - nof_deleted_buffers > nof_free_buffers)
    {
        *hbm_is_empty = FALSE;
    }
    else if (nof_buffers - nof_deleted_buffers < nof_free_buffers)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "number of free buffers exceed number of buffers in the system, something is wrong\n");
        *hbm_is_empty = FALSE;
    }
    else
    {
        *hbm_is_empty = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_hbmc_temp_monitor(
    int unit,
    void *user_data)
{
    dnx_dram_temperature_monitoring_state_e previous_state;
    periodic_event_handler_t temp_monitor_handler;
    int temp = 0;
    uint8 hbm_is_empty;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get previous state from SW-State
     */
    SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.get(unit, &previous_state));

    /*
     * Get current temperature of HBM
     */
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_temp_get(unit, DNX_HBMC_ITER_ALL, &temp),
                             "HBM temperature monitoring failed, temp read was not successful %s%s%s\n", EMPTY, EMPTY,
                             EMPTY);

    if (temp >= dnx_data_dram.hbm.power_down_temp_threshold_get(unit))
    {
        /*
         * If temperature exceed power down threshold
         * call power down routine
         */
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "HBM temperature exceed power down threshold - %d C, shutting down the HBM\n"),
                  dnx_data_dram.hbm.power_down_temp_threshold_get(unit)));
        /*
         * Stop traffic to HBM
         */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_traffic_to_hbm_stop(unit),
                                 "HBM temperature monitoring failed%s%s%s\n", EMPTY, EMPTY, EMPTY);

        /*
         * Shut down the temperature monitoring
         */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_power_down(unit),
                                 "HBM power down failed, failed to shut down power to HBM%s%s%s\n", EMPTY, EMPTY,
                                 EMPTY);

        /*
         *  Stop Temperature monitoring next time it is triggered
         */
        SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_handler.get(unit, &temp_monitor_handler));
        SHR_IF_ERR_EXIT(periodic_event_stop(temp_monitor_handler, 0, 1));

        /*
         * Check if HBM was emptied
         */
        SHR_IF_ERR_EXIT(dnx_hbmc_traffic_is_empty_get(unit, &hbm_is_empty));

        /*
         * If HBM was NOT emptied before shut down
         * need to hard reset
         */
        if (!hbm_is_empty)
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "HBM failed to get empty prior to shutting down, Hard reset is required.\n");
        }
    }
    else if ((previous_state == DNX_DRAM_NORMAL_TEMP)
             && (temp >= dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit)))
    {
        /*
         * If temperature exceed stop traffic threshold
         * stop traffic to HBM
         */
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit, "HBM temperature exceed stop traffic threshold - %d C, traffic is routed through OCB only\n"),
                  dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit)));

        /*
         * stop traffic to HBM
         */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_traffic_to_hbm_stop(unit),
                                 "HBM temperature monitoring failed%s%s%s\n", EMPTY, EMPTY, EMPTY);

        /*
         * checking that the HBM was emptied is not required here,
         * if the device will reach a state when it needs to shut down the HBM,
         * a check will be made then to see if the HBM was emptied or not and act accordingly
         */

        /*
         * set state to high (1)
         */
        SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.set(unit, DNX_DRAM_HIGH_TEMP));
    }
    else if ((previous_state == DNX_DRAM_HIGH_TEMP)
             && (temp <= dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit)))
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "HBM temperature dropped below restore traffic threshold - %d C, traffic can be routed through both HBM and OCB\n"),
                  dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit)));

        /** restore traffic to HBM  */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_traffic_to_hbm_restore(unit),
                                 "HBM temperature monitoring failed\n%s%s%s", EMPTY, EMPTY, EMPTY);

        /*
         * set state to normal (0)
         */
        SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.set(unit, DNX_DRAM_NORMAL_TEMP));
    }

exit:
    SHR_FUNC_EXIT;
}

/* see header file */
shr_error_e
dnx_hbmc_temp_stub_set(
    int unit,
    int nof_elements,
    uint32 *temp_arr)
{
    dnx_hbmc_temp_stub_t *cur_element = NULL;
    dnx_hbmc_temp_stub_t *temp_element = NULL;
    dnx_hbmc_temp_stub_t *first_element = NULL;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    /** if dnx_hbmc_temp_stub_head != NULL - return error, because it is in the middle of operation */
    if (dnx_hbmc_temp_stub_head[unit] != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx_hbmc_temp_stub_head!=NULL, cannot configure new stub\n");
    }
    /** if nof_elements = 0 - go to exit */
    if (nof_elements == 0)
    {
        SHR_EXIT();
    }

    /** set the link list */
    for (i = 0; i < nof_elements; i++)
    {
        temp_element = (dnx_hbmc_temp_stub_t *) sal_alloc(sizeof(dnx_hbmc_temp_stub_t), "dnx_hbm_tem_stub");
        if (temp_element == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "sal_alloc returned NULL pointer for index=%d\n", i);
        }
        temp_element->value = temp_arr[i];
        temp_element->next = NULL;
        /** save first, but still don't set it as the head until finish filling the link-list */
        if (i == 0)
        {
            first_element = temp_element;
        }
        if (cur_element != NULL)
        {
            cur_element->next = temp_element;
        }
        cur_element = temp_element;
    }

    dnx_hbmc_temp_stub_head[unit] = first_element;

exit:
    SHR_FUNC_EXIT;
}
