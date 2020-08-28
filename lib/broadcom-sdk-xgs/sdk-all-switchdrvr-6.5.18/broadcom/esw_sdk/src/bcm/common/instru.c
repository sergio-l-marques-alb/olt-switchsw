/*
 * $Id: switch.c,v 1.16 2018/23/03 20:49:16 Krum Kotsev Exp $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:       instru.c
 * Purpose:    Instrumentation common APIs
 */

#include <soc/drv.h>
#include <bcm/instru.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/control.h>

#if defined(BCM_PETRA_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT)
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif

#if (defined(BCM_DPP_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#define SOC_SYNC_WHEN_NOT_DIRTY ((SOC_IS_DPP(unit) || SOC_IS_DFE(unit)) ? 1 : 0)
#else
#define SOC_SYNC_WHEN_NOT_DIRTY 0
#endif

/*
 * Function:
 *     bcm_switch_pkt_info_t_init
 * Description:
 *     Initialize an switch packet parameter structure
 * Parameters:
 *     pkt_info - pointer to switch packet parameter structure
 * Return: none
 */
extern void
bcm_instru_ipt_t_init(bcm_instru_ipt_t *config)
{
    sal_memset(config, 0, sizeof(bcm_instru_ipt_t));
}

