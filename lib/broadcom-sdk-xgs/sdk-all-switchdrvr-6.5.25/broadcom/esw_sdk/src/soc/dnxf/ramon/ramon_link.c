 /*
  * $Id: ramon_link.c,v 1.3 Broadcom SDK $
  *
  * $Copyright: (c) 2021 Broadcom.
  * Broadcom Proprietary and Confidential. All rights reserved.$
  *
  * File:        ramon_link.c
  * Purpose:     Hardware Linkscan module
  *
  * These routines will be called by the linkscan module,
  * so they need to be defined and return _SHR_E_NONE.
  */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/linkctrl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmicm.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/cmicm.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/ramon/ramon_link.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>

/*
 * Function:
 *     _soc_ramon_linkctrl_linkscan_hw_init
 * Purpose:
 *     Initialize hardware linkscan.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_ramon_linkctrl_linkscan_hw_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_linkscan_ports_write(
    int unit,
    pbmp_t hw_mii_pbm)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_config(unit, &hw_mii_pbm));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     _soc_ramon_linkctrl_linkscan_pause
 * Purpose:
 *     Pause link scanning, without disabling it.
 *     This call is used to pause scanning temporarily.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_ramon_linkctrl_linkscan_pause(
    int unit)
{
    uint32 m0_pause_enable;
    SHR_FUNC_INIT_VARS(unit);

    m0_pause_enable = dnxf_data_linkscan.general.m0_pause_enable_get(unit);
    if (!m0_pause_enable)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_pause(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     _soc_ramon_linkctrl_linkscan_continue
 * Purpose:
 *     Continue link scanning after it has been paused.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_ramon_linkctrl_linkscan_continue(
    int unit)
{
    uint32 m0_pause_enable;
    SHR_FUNC_INIT_VARS(unit);

    m0_pause_enable = dnxf_data_linkscan.general.m0_pause_enable_get(unit);
    if (!m0_pause_enable)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_continue(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     _soc_ramon_linkctrl_linkscan_config
 * Purpose:
 *     Set ports to hardware linkscan.
 * Parameters:
 *     unit          - Device number
 *     hw_mii_pbm    - Port bit map of ports to scan with MIIM registers
 *     hw_direct_pbm - Port bit map of ports to scan using NON MII
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_ramon_linkctrl_linkscan_config(
    int unit,
    pbmp_t hw_mii_pbm,
    pbmp_t hw_direct_pbm)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    pbmp_t pbm;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if disabling port scanning 
     */

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_OR(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm))
    {
        /*
         * NOTE: we are no longer using CC_LINK_STAT_EN since it is
         * unavailable on 5695 and 5665.  EPC_LINK will be updated by
         * software anyway, it will just take a few extra milliseconds.
         */
        SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) | SOC_F_LSE);
    }
    else
    {
        SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) & ~SOC_F_LSE);
    }

    SHR_IF_ERR_EXIT(_soc_ramon_linkscan_ports_write(unit, hw_mii_pbm));

    SOC_PBMP_ASSIGN(soc->hw_linkscan_pbmp, hw_mii_pbm);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:    
 *     _soc_ramon_linkctrl_update
 * Purpose:
 *     Update the forwarding state in device.
 * Parameters:  
 *      unit - Device unit number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_ramon_linkctrl_update(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_linkctrl_hw_link_get(
    int unit,
    soc_pbmp_t * hw_link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(hw_link, _SHR_E_PARAM, "hw_link");
    /*
     * NOTE: We must clear hwlink, otherwise,there 
     * will be link-up info about the previous port.
     */
    SOC_PBMP_CLEAR(*hw_link);
    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_hw_link_cache_get(unit, hw_link));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Link Control Driver - RAMON
 */
CONST soc_linkctrl_driver_t soc_linkctrl_driver_ramon = {
    NULL,       /* port mapping */
    _soc_ramon_linkctrl_linkscan_hw_init,       /* ld_linkscan_hw_init */
    _soc_ramon_linkctrl_linkscan_config,        /* ld_linkscan_config */
    _soc_ramon_linkctrl_linkscan_pause, /* ld_linkscan_pause */
    _soc_ramon_linkctrl_linkscan_continue,      /* ld_linkscan_continue */
    _soc_ramon_linkctrl_update, /* ld_update */
    _soc_ramon_linkctrl_hw_link_get     /* ld_hw_link_get */
};

shr_error_e
soc_ramon_linkctrl_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_linkctrl_init(unit, &soc_linkctrl_driver_ramon));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
