/*
 * $Id: cosq.c,v 1.1 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:    cosq.c
 * Purpose: Manages common COSQ functions
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/debug.h>

#include <bcm/port.h>
#include <bcm/cosq.h>

/*
 * Function:
 *      bcm_cosq_gport_discard_t_init
 * Purpose:
 *      Initialize the COSQ gport discard struct
 * Parameters:
 *      discard - Pointer to the struct to be init'ed
 */

void
bcm_cosq_gport_discard_t_init(bcm_cosq_gport_discard_t *discard)
{
    if (discard != NULL) {
        sal_memset(discard, 0, sizeof(*discard));
    }
}

/*
 * Function:
 *      bcm_cosq_congestion_mapping_info_t_init
 * Purpose:
 *      Initialize the COSQ congestion mapping struct
 * Parameters:
 *      config - Pointer to the struct to be init'ed
 */
void
bcm_cosq_congestion_mapping_info_t_init(bcm_cosq_congestion_mapping_info_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));
    }
}

/*
 * Function:
 *      bcm_cosq_classifier_t_init
 * Purpose:
 *      Initialize the COSQ classifier struct
 * Parameters:
 *      classifier - Pointer to the struct to be init'ed
 */

void
bcm_cosq_classifier_t_init(bcm_cosq_classifier_t *classifier)
{
    if (classifier != NULL) {
        sal_memset(classifier, 0, sizeof(*classifier));
    }
}

/*
 * Function:
 *      bcm_cosq_congestion_info_t_init
 * Purpose:
 *      Initialize the COSQ congestion struct
 * Parameters:
 *      config - Pointer to the struct to be init'ed
 */

void
bcm_cosq_congestion_info_t_init(bcm_cosq_congestion_info_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));

        /* Setting the default values to -1 since the validation
         * is done for -1 in all the related APIs.
         */
        config->fabric_modid = -1;
        config->fabric_port = -1;
        config->dest_modid = -1;
        config->dest_port = -1;
    }
}

/*
 * Function:
 *      bcm_cosq_bst_profile_t_init
 * Purpose:
 *      Initialize a CoSQ BST profile structure.
 * Parameters:
 *      profile - Pointer to the struct to be init'ed
 */

void
bcm_cosq_bst_profile_t_init(bcm_cosq_bst_profile_t *profile)
{
    if (profile != NULL) {
        sal_memset(profile, 0, sizeof(*profile));
    }
}
void 
bcm_cosq_threshold_t_init(bcm_cosq_threshold_t *threshold) {
    if (threshold != NULL) {
        sal_memset(threshold, 0, sizeof(*threshold));
        threshold->valid = BCM_COSQ_THRESHOLD_VALID_DP | BCM_COSQ_THRESHOLD_VALID_VALUE;
    }
}

/*
 * Function:
 *      bcm_cosq_pfc_class_mapping_t_init
 * Purpose:
 *      Initialize a PFC class mapping structure.
 * Parameters:
 *      mapping - Pointer to the struct to be init'ed
 */
void
bcm_cosq_pfc_class_mapping_t_init(bcm_cosq_pfc_class_mapping_t *mapping)
{
    int index = 0;

    if (mapping == NULL) {
        return;
    }
    
    sal_memset(mapping, 0, sizeof(*mapping));
    for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
        mapping->gport_list[index] = BCM_GPORT_INVALID;
    }
}

/*
 * Function:
 *      bcm_cosq_safc_class_mapping_t_init
 * Purpose:
 *      Initialize a SAFC class mapping structure.
 * Parameters:
 *      mapping - Pointer to the struct to be init'ed
 */
void
bcm_cosq_safc_class_mapping_t_init(bcm_cosq_safc_class_mapping_t *mapping)
{
    int index = 0;

    if (mapping == NULL) {
        return;
    }

    sal_memset(mapping, 0, sizeof(*mapping));
    for (index = 0; index < BCM_COSQ_SAFC_GPORT_COUNT; index++) {
        mapping->gport_list[index] = BCM_GPORT_INVALID;
    }
}

/*
 * Function:
 *      bcm_cosq_service_pool_t_init
 * Purpose:
 *      Initialize a COSQ service pool structure.
 * Parameters:
 *      service_pool - Pointer to the struct to be init'ed
 */

void
bcm_cosq_service_pool_t_init(bcm_cosq_service_pool_t *service_pool)
{
    if (service_pool != NULL) {
        sal_memset(service_pool, 0, sizeof(*service_pool));
    }
}

