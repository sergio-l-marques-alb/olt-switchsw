/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/collector.h>
#include <bcm/telemetry.h>

/* Initialize a telemetry config information structure. */
void bcm_telemetry_config_t_init(bcm_telemetry_config_t
                                     *telemetry_config)
{
    sal_memset(telemetry_config, 0, sizeof(bcm_telemetry_config_t));
}

