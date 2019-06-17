 /* 
 * $Id: dnx_linkctrl.c,v 1.3 2013/06/18 15:46:37 Exp $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        dnx_linkctrl.c
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

#include <soc/error.h>

#include <soc/dnx/dnx_linkctrl.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/shrextend/shrextend_debug.h>


/*
 * Function:
 *     _soc_dnx_linkctrl_linkscan_hw_init
 * Purpose:
 *     Initialize hardware linkscan.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_dnx_linkctrl_linkscan_hw_init(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_dnx_linkscan_ports_write(int unit, pbmp_t hw_mii_pbm)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_config(unit, hw_mii_pbm));

exit:

    SHR_FUNC_EXIT;  
}

/*
 * Function:
 *     _soc_dnx_linkctrl_linkscan_pause
 * Purpose:
 *     Pause link scanning, without disabling it.
 *     This call is used to pause scanning temporarily.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_dnx_linkctrl_linkscan_pause(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_pause(unit));

exit:

    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     _soc_dnx_linkctrl_linkscan_continue
 * Purpose:
 *     Continue link scanning after it has been paused.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_dnx_linkctrl_linkscan_continue(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(_soc_dnx_linkscan_ports_write(unit, soc->hw_linkscan_pbmp));

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_continue(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *     _soc_dnx_linkctrl_linkscan_config
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
_soc_dnx_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm,
                                    pbmp_t hw_direct_pbm)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    pbmp_t pbm;

    SHR_FUNC_INIT_VARS(unit);

    /* Check if disabling port scanning */

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_OR(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm)) {
       /*
        * NOTE: we are no longer using CC_LINK_STAT_EN since it is
        * unavailable on 5695 and 5665.  EPC_LINK will be updated by
        * software anyway, it will just take a few extra milliseconds.
        */
       soc->soc_flags |= SOC_F_LSE;
    } else {
        soc->soc_flags &= ~SOC_F_LSE;
    }

    /* The write of the HW linkscan ports is moved to the linkscan
     * continue below.  Note that though the continue function
     * will not write to the CMIC scan ports register if linkscan
     * was disabled above, that is only the case when the port bitmap
     * is empty.  Since linkscan pause clears the bitmap, this is the
     * desired result.
     */
    SOC_PBMP_ASSIGN(soc->hw_linkscan_pbmp, hw_mii_pbm);

    SHR_FUNC_EXIT;
}

/*
 * Function:    
 *     _soc_dnx_linkctrl_update
 * Purpose:
 *     Update the forwarding state in device.
 * Parameters:  
 *      unit - Device unit number
 * Returns:
 *     _SHR_E_NONE
 */
STATIC int
_soc_dnx_linkctrl_update(int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

STATIC int
_soc_dnx_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(hw_link, _SHR_E_PARAM, "hw_link");

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(soc_cmicx_linkscan_hw_link_get(unit, hw_link));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Link Control Driver - dnx
 */
CONST soc_linkctrl_driver_t  soc_linkctrl_driver_dnx = {
    NULL,                                    /* port mapping */
    _soc_dnx_linkctrl_linkscan_hw_init,      /* ld_linkscan_hw_init */
    _soc_dnx_linkctrl_linkscan_config,       /* ld_linkscan_config */
    _soc_dnx_linkctrl_linkscan_pause,        /* ld_linkscan_pause */
    _soc_dnx_linkctrl_linkscan_continue,     /* ld_linkscan_continue */
    _soc_dnx_linkctrl_update,                /* ld_update */
    _soc_dnx_linkctrl_hw_link_get            /* ld_hw_link_get */

};

 
shr_error_e
soc_dnx_linkctrl_init(int unit) 
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_linkctrl_init(unit, &soc_linkctrl_driver_dnx));

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE
