/*
 * $Id: debug.c,v 1.9 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>

#include <soc/dnx/legacy/debug.h>
#include <soc/dnx/legacy/drv.h>

#include <soc/dnx/legacy/ARAD/arad_debug.h>
#include <shared/shrextend/shrextend_debug.h>

shr_error_e soc_dnx_dbg_egress_shaping_enable_get(const unsigned int unit, uint8 *enable)
{
  int rv = _SHR_E_NONE;
    rv = jer2_arad_dbg_egress_shaping_enable_get(unit, enable);
    return rv;
}

shr_error_e soc_dnx_dbg_egress_shaping_enable_set(const unsigned int unit, const uint8 enable)
{
  int rv = _SHR_E_NONE;
  rv = jer2_arad_dbg_egress_shaping_enable_set(unit, enable);
  return rv;
}

#undef BSL_LOG_MODULE

