/* 
 * $Id: bcmx_policer.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        bcmx/policer.c
 * Purpose:     BCMX Policer APIs
 *
 */

#include <bcm/types.h>

#include <bcmx/bcmx.h>
#include <bcmx/policer.h>

#include "bcmx_int.h"

#define BCMX_POLICER_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_policer_config_t *)(_config))


/*
 * Function:
 *     bcmx_policer_config_t_init
 */
void
bcmx_policer_config_t_init(bcmx_policer_config_t *pol_cfg)
{
    if (pol_cfg != NULL) {
        bcm_policer_config_t_init(BCMX_POLICER_CONFIG_T_PTR_TO_BCM(pol_cfg));
    }
}

/*
 * Function:
 *     bcmx_policer_init
 */
int
bcmx_policer_init(void)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_init(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_create
 * Notes:
 *     If policer ID is not specified, the ID returned
 *     from the first successful 'create' is used for the remaining
 *     units.
 */
int
bcmx_policer_create(bcmx_policer_config_t *pol_cfg, 
                    bcm_policer_t *policer_id)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(pol_cfg);
    BCMX_PARAM_NULL_CHECK(policer_id);

    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_policer_create(bcm_unit,
                                    BCMX_POLICER_CONFIG_T_PTR_TO_BCM
                                    (pol_cfg),
                                    policer_id);
        /*
         * Use the ID from first successful 'create' if policer ID
         * is not specified.
         */
        if (!(pol_cfg->flags & BCM_POLICER_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                pol_cfg->flags |= BCM_POLICER_WITH_ID;
            }
        }

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;

}

/*
 * Function:
 *     bcmx_policer_destroy
 */
int
bcmx_policer_destroy(bcm_policer_t policer_id)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_destroy(bcm_unit, policer_id);
        if (tmp_rv == BCM_E_NOT_FOUND) {
            tmp_rv = BCM_E_NONE;
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_destroy_all
 */
int
bcmx_policer_destroy_all(void)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_destroy_all(bcm_unit);
        if (tmp_rv == BCM_E_NOT_FOUND) {
            tmp_rv = BCM_E_NONE;
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_set
 */
int
bcmx_policer_set(bcm_policer_t policer_id, 
                 bcmx_policer_config_t *pol_cfg)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(pol_cfg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_policer_set(bcm_unit, policer_id,
                                 BCMX_POLICER_CONFIG_T_PTR_TO_BCM
                                 (pol_cfg));
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_policer_get
 */
int
bcmx_policer_get(bcm_policer_t policer_id,
                 bcmx_policer_config_t *pol_cfg)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(pol_cfg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_policer_get(bcm_unit,  policer_id,
                             BCMX_POLICER_CONFIG_T_PTR_TO_BCM(pol_cfg));
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

