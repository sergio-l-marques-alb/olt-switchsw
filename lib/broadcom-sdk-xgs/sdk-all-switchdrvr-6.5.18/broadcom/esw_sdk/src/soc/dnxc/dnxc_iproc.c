/*
 * $Id: dnxc_iproc.c,v 1.0 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
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

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif /* BCM_IPROC_SUPPORT */

#include <soc/drv.h>
#include <soc/dnxc/dnxc_iproc.h>
#include <soc/dnxc/error.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_IPROC_SUPPORT

/* 
 * Configure PAXB: the iProc PCIe-AXI bridge 
 * This code was used to configure PCIe function 0.
 * The configuration of the second bar moved elsewhere, and the third bar is not used by the hardware any more.
 */
int
soc_dnxc_iproc_config_paxb(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif /* BCM_IPROC_SUPPORT */
