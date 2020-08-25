/*! \file tx.c
 * $Id$
 *
 * TX procedures for DNX.
 *
 * Here add DESCRIPTION.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/common/tx.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/tx.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#ifdef ADAPTER_SERVER_MODE
#include <soc/sand/sand_signals.h>
#include <soc/dnx/adapter/adapter_reg_access.h>

#if ! (defined __KERNEL__) && ! (defined _STDLIB_H)
extern char *getenv(
    const char *);
#endif

#endif
/*
 * }
 */

int
bcm_dnx_tx(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
#ifdef ADAPTER_SERVER_MODE
    uint32 adapter_loopback_enable;
    adapter_ms_id_e ms_id = ADAPTER_MS_ID_FIRST_MS;
#endif

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_SUPPRESS(unit);

#ifdef ADAPTER_SERVER_MODE

    /*
     * Clear Signal Cache 
     */
    sand_adapter_clear_signals(unit);

  /** Gets the value of the adapter_loopback_enable from the dnx_data */
    adapter_loopback_enable = dnx_data_adapter.tx.loopback_enable_get(unit);

    if (adapter_loopback_enable == 1)
    {
        ms_id = ADAPTER_MS_ID_LOOPBACK;
    }

    
    SHR_IF_ERR_EXIT(adapter_set_mem_access_recording(unit, 0));
    /*
     * Use a different flow for sending a packet in adapter.
     * The first two entries in the packet data contain a hard coded value for verification and the tx port
     * We therefor pass the data starting at index 2 and use index 1 to pass the tx port
     */

    SHR_IF_ERR_EXIT(adapter_send_buffer(unit, (uint32) ms_id, (uint32) (pkt->src_gport),
                                        pkt->pkt_data->len, pkt->pkt_data->data, 1));

    /*
     * Wait 15 seconds (for remote server only) and stop the hit bit recording.
     */
    if (getenv("SOC_TARGET_SERVER"))
    {
        sal_usleep(15000000);
    }
    else
    {
        sal_usleep(6500000);
    }
    SHR_IF_ERR_EXIT(adapter_set_mem_access_recording(unit, 0));

#else

    /** Regular TX handling */
    /*
     * Clear Debug Signals Memory - return status irrelevant
     */
    if (!(pkt->flags & BCM_TX_NO_VISIBILITY_RESUME))
    {
        dnx_visibility_resume(unit, BCM_CORE_ALL,
                              BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                              DNX_VISIBILITY_RESUME_MEM);
    }

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pointer to pkt is not valid");
    SHR_IF_ERR_EXIT(bcm_common_tx(unit, pkt, cookie));

#endif

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}
