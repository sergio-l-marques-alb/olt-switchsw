/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INT - In-band Telemetry Turnaround Embedded Application APP interface
 * Purpose: API to configure In-band Telemetry Turnaround embedded app interface.
 *
 */
#if defined(INCLUDE_INT)
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/int.h>

/* Initialize INT Turnaround configuration information structure. */
void bcm_int_turnaround_config_t_init(bcm_int_turnaround_config_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_int_turnaround_config_t));
}
#else /* INCLUDE_INT */
typedef int _int_turnaround_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_INT */
