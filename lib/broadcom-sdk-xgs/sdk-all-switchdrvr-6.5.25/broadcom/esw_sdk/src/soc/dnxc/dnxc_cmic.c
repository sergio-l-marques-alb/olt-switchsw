/*
 * $Id: dnxc_cmic.c,v 1.0 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXC IPROC
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* BSL_LOG_MODULE */

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/dnxc/error.h>
#include <shared/shrextend/shrextend_debug.h>

int
soc_dnxc_cmic_sbus_timeout_set(
    int unit,
    uint32 core_freq_khz,
    int schan_timeout)
{

    uint32 freq_mhz = core_freq_khz / 1000;
    uint32 ticks, max_uint = 0xFFFFFFFF, max_ticks = 0x3FFFFF;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * configure ticks to be a HW timeout that is 75% of SW timeout. units: schanTimeout is in microsecond frequency is 
     * recieved in KHz, and modified to be in MHz.  after the modification: ticks = frequency * Timeout 
     */

    if ((max_uint / freq_mhz) > schan_timeout)
    {   /* make sure ticks can be represented in 32 bits */
        ticks = freq_mhz * schan_timeout;
        ticks = ((ticks / 100) * 75);   /* make sure hardware timeout is smaller than software */
    }
    else
    {
        ticks = max_ticks;
    }

    SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_TIMEOUTr(unit, ticks));

exit:
    SHR_FUNC_EXIT;
}
