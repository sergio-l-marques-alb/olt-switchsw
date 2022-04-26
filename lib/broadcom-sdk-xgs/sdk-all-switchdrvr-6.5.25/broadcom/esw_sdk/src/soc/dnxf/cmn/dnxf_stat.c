
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF STAT
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#ifdef BCM_DNXF_SUPPORT
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <bcm_int/dnxf_dispatch.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_stat.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

/*
 * Function:
 *      soc_dnxf_stat_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number
 *      counters - (IN) array of counters to be summed
 *      array size- (IN) size of the counters array
 *      value - (OUT)    Counter value
 * Returns:
 *      _SHR_E_NONE       Success.
 *      _SHR_E_PARAM      Illegal parameter.
 *      _SHR_E_BADID      Illegal port number.
 *      _SHR_E_INTERNAL   Device access failure.
 *      _SHR_E_UNAVAIL    Counter/variable is not implemented on this current chip.
 */

shr_error_e
soc_dnxf_stat_get(
    int unit,
    bcm_port_t port,
    uint64 *value,
    int *counters,
    int arr_size)
{
    int i = 0;
    uint64 val64;
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ZERO(*value);
    for (i = 0; i < arr_size; i++)
    {
        if (COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[counters[i]]))
        {
            if (SOC_CONTROL(unit)->counter_interval != 0)
            {
                /** counter is collected by counter thread */
                SHR_IF_ERR_EXIT(soc_counter_get(unit, port, counters[i], 0, &val64));
            }
            else
            {
                /*
                 * counter isn't collected by counter thread
                 */
                SHR_IF_ERR_EXIT(SOC_CONTROL(unit)->controlled_counters[counters[i]].controlled_counter_f(unit,
                                                                                                         SOC_CONTROL
                                                                                                         (unit)->controlled_counters
                                                                                                         [counters
                                                                                                          [i]].counter_id,
                                                                                                         port, &val64,
                                                                                                         NULL));
            }
        }
        else
        {
            /*
             * counter isn't collected by counter thread
             */
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                            (unit, mbcm_dnxf_controlled_counter_get, (unit, counters[i], port, &val64, NULL)));
        }
        COMPILER_64_ADD_64(*value, val64);

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fabric_stat_verify(
    int unit,
    int link,
    int pipe)
{
    int nof_pipes, nof_links;
    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.max_nof_pipes_get(unit);
    nof_links = dnxf_data_port.general.nof_links_get(unit);

    if (pipe > _SOC_DNXF_FABRIC_STAT_PIPE_NO_VERIFY)
    {
        SHR_RANGE_VERIFY(pipe, 0, nof_pipes - 1, _SHR_E_PARAM, "pipe %d argument is out of range.", pipe);
    }

    if (link >= nof_links)
    {
        SHR_RANGE_VERIFY(link, 0, nof_links - 1, _SHR_E_PARAM, "link %d argument is out of range.", link);
    }

exit:
    SHR_FUNC_EXIT;
}

#endif
