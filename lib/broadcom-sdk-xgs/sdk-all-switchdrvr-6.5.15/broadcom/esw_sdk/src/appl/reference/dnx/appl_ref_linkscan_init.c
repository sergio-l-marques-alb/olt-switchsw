/** \file appl_ref_linkscan_init.c
 * $Id$
 *
 * Linkscan application procedures for DNX. 
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINKSCANDNX

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#ifdef DNX_EMULATION_1_CORE
#include <soc/sand/sand_aux_access.h>
#endif

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/**
 * \brief - Initialize linkscan
 * 
 * \param [in] unit - unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_linkscan_init(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    uint32 linkscan_interval = BCM_LINKSCAN_INTERVAL_DEFAULT;
    bcm_port_config_t pcfg;
    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing Linkscan application in emulation%s%s%s\n", EMPTY, EMPTY,
                          EMPTY);
    }
#endif

    /*
     * in warmboot will be init by the BCM 
     */
    if (SOC_WARM_BOOT(unit))
    {
        int interval;

        rv = bcm_linkscan_enable_get(unit, &interval);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "read Linkscan interval Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);

        rv = bcm_linkscan_enable_set(unit, interval);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Init Linkscan Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    else
    {
        /*
         * Turn linkscan on 
         */
        rv = bcm_linkscan_enable_set(unit, linkscan_interval);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "bcm_linkscan_enable_set() Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);

        rv = bcm_port_config_get(unit, &pcfg);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "bcm ports not initialized\n%s%s%s", EMPTY, EMPTY, EMPTY);

        /*
         * Adding nif ports to linkscan
         */
        BCM_PBMP_ASSIGN(pbmp, pcfg.nif);

        rv = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_SW);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "bcm_linkscan_mode_set_pbm() Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_linkscan_deinit(
    int unit)
{
    int rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rv = bcm_linkscan_detach(unit);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "bcm_linkscan_detach() Failed:\n%s%s%s", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}
