/*
 * $Id$
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Utility routines for Bond Options Init & De-Init
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT)

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/bondoptions.h>

soc_bond_info_t *soc_bond_info[SOC_MAX_NUM_DEVICES];



/*
 * Function:
 *      soc_bond_info_init
 * Purpose:
 *      Initialize SOC bond options information cache.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_bond_info_init(int unit)
{
    int rv = SOC_E_NONE;
    uint16 dev_id, driver_dev_id;
    uint8 rev_id, driver_rev_id;


    if (SOC_BOND_INFO(unit) == NULL) {
        SOC_BOND_INFO(unit) = sal_alloc(sizeof(soc_bond_info_t), "SOC Bond Info Structure");
        if (SOC_BOND_INFO(unit) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(SOC_BOND_INFO(unit), 0, (sizeof(soc_bond_info_t)));

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &driver_dev_id, &driver_rev_id);

    /* Call bond_options_init function only if defined */
    if (SOC_FUNCTIONS(unit)->soc_bond_options_init) {
        rv = SOC_FUNCTIONS(unit)->soc_bond_options_init(unit);
    }

    return rv;
}


/*
 * Function:
 *      soc_bond_info_deinit
 * Purpose:
 *      UnInitialize/Deallocate SOC bond options information cache.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_bond_info_deinit(int unit)
{
    sal_free(SOC_BOND_INFO(unit));
    SOC_BOND_INFO(unit) = NULL;

    return SOC_E_NONE;
}

#else
int _soc_common_bondoptionc_c_not_empty;
#endif /* BCM_ESW_SUPPORT && BCM_IPROC_SUPPORT */

