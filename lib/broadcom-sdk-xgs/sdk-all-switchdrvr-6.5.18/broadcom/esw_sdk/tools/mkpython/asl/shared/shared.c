/*! \file shared.c
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>
#include <shared/bslext.h>

static int
simple_bsl_out_hook(bsl_meta_t * meta, const char *format, va_list args)
{
    return sal_vprintf(format, args);
}

/* This threshold can be changed to see more levels of messages */
bsl_severity_t currentBslSeverityThreshold = bslSeverityError;

static int
simple_bsl_check_hook(bsl_packed_meta_t meta_pack)
{
    const int   severity = BSL_SEVERITY_GET(meta_pack);

    return (severity <= currentBslSeverityThreshold);
}

/* Call this routine early on to enable all messaged printed by the SDK. */
void
bcm_bsl_setup(bsl_severity_t level)
{
    bsl_config_t bsl_config;

    currentBslSeverityThreshold = level;

    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = simple_bsl_out_hook;
    bsl_config.check_hook = simple_bsl_check_hook;
    bsl_init(&bsl_config);
}
