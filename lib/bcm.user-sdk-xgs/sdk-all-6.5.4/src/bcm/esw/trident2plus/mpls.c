/*
 * $Id: $
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
 * File:    mpls.c
 * Purpose: Handle trident2plus specific MPLS APIs
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#ifdef INCLUDE_L3
#ifdef BCM_MPLS_SUPPORT
#ifdef BCM_TRIDENT2PLUS_SUPPORT

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/trident2plus.h>

/*
 * Function:
 *      bcm_mpls_entropy_identifier_add
 * Purpose:
 *      Add an MPLS entropy label identifier
 * Parameters:
 *      unit - Device Number
 *      info - Entropy Label information
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2p_mpls_entropy_identifier_add(
    int unit, 
    uint32 options, 
    bcm_mpls_entropy_identifier_t *info)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    mpls_entropy_label_data_entry_t label_data;

    if (!BCM_TD2P_ENTROPY_LABEL_ID_VALID(info->label) ||
            !BCM_TD2P_ENTROPY_LABEL_MASK_VALID(info->mask)) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(val64);
    BCM_IF_ERROR_RETURN(READ_MPLS_ENTROPY_LABEL_CONFIG_64r(unit, &val64));
    soc_reg64_field32_set(unit, MPLS_ENTROPY_LABEL_CONFIG_64r, &val64,
            VALIDf, 1);
    soc_reg64_field32_set(unit, MPLS_ENTROPY_LABEL_CONFIG_64r, &val64,
            VALUEf, info->label);
    soc_reg64_field32_set(unit, MPLS_ENTROPY_LABEL_CONFIG_64r, &val64,
            MASKf, info->mask);
    BCM_IF_ERROR_RETURN(WRITE_MPLS_ENTROPY_LABEL_CONFIG_64r(unit, val64));

    sal_memset(&label_data, 0, sizeof(label_data));
    soc_MPLS_ENTROPY_LABEL_DATAm_field32_set(unit, &label_data, VALIDf, 1);
    soc_MPLS_ENTROPY_LABEL_DATAm_field32_set(unit, &label_data,
            MPLS_ACTION_IF_NOT_BOSf, 1);
    WRITE_MPLS_ENTROPY_LABEL_DATAm(unit, MEM_BLOCK_ALL, 0, &label_data);

    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_delete
 * Purpose:
 *      Delete an MPLS entropy label identifier
 *      If no label information is passed, return error.
 * Parameters:
 *      unit - Device Number
 *      info - Entropy Label information
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2p_mpls_entropy_identifier_delete(
    int unit, 
    bcm_mpls_entropy_identifier_t *info)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    int valid;
    bcm_mpls_label_t entropy_label;
    bcm_mpls_label_t entropy_mask;
    mpls_entropy_label_data_entry_t label_data;

    BCM_IF_ERROR_RETURN(READ_MPLS_ENTROPY_LABEL_CONFIG_64r(unit,
                &val64));
    valid = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALIDf);
    if (!valid) {
        return BCM_E_EMPTY;
    }

    if (!BCM_TD2P_ENTROPY_LABEL_ID_VALID(info->label) ||
            !BCM_TD2P_ENTROPY_LABEL_MASK_VALID(info->mask)) {
        return BCM_E_PARAM;
    } else {
        entropy_label = soc_reg64_field32_get(unit,
                MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALUEf);
        entropy_mask = soc_reg64_field32_get(unit,
                MPLS_ENTROPY_LABEL_CONFIG_64r, val64, MASKf);
        if ((info->label != entropy_label) || (info->mask != entropy_mask)) {
            return BCM_E_NOT_FOUND;
        }
    }

    COMPILER_64_ZERO(val64);
    BCM_IF_ERROR_RETURN(WRITE_MPLS_ENTROPY_LABEL_CONFIG_64r(unit, val64));

    sal_memset(&label_data, 0, sizeof(label_data));
    WRITE_MPLS_ENTROPY_LABEL_DATAm(unit, MEM_BLOCK_ALL, 0, &label_data);

    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_delete_all
 * Purpose:
 *      Delete all MPLS entropy label identifier entries.
 * Parameters:
 *      unit   - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2p_mpls_entropy_identifier_delete_all(
    int unit)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    mpls_entropy_label_data_entry_t label_data;

    COMPILER_64_ZERO(val64);
    BCM_IF_ERROR_RETURN(WRITE_MPLS_ENTROPY_LABEL_CONFIG_64r(unit, val64));

    sal_memset(&label_data, 0, sizeof(label_data));
    WRITE_MPLS_ENTROPY_LABEL_DATAm(unit, MEM_BLOCK_ALL, 0, &label_data);

    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_get
 * Purpose:
 *      Get an MPLS entropy label identifier entry
 *      If no label info is passed, return the only configured
 *      entropy label.
 * Parameters:
 *      unit - Device Number
 *      info - Entropy Label information
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2p_mpls_entropy_identifier_get(
    int unit, 
    bcm_mpls_entropy_identifier_t *info)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    int valid;
    bcm_mpls_label_t entropy_label;
    bcm_mpls_label_t entropy_mask;

    BCM_IF_ERROR_RETURN(READ_MPLS_ENTROPY_LABEL_CONFIG_64r(unit,
                &val64));
    valid = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALIDf);
    if (!valid) {
        return BCM_E_EMPTY;
    }

    if (BCM_TD2P_ENTROPY_LABEL_ID_VALID(info->label) &&
            BCM_TD2P_ENTROPY_LABEL_MASK_VALID(info->mask)) {
        entropy_label = soc_reg64_field32_get(unit,
                MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALUEf);
        entropy_mask = soc_reg64_field32_get(unit,
                MPLS_ENTROPY_LABEL_CONFIG_64r, val64, MASKf);
        if ((info->label & info->mask) != (entropy_label & entropy_mask)) {
            return BCM_E_NOT_FOUND;
        }
    }

    info->label = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALUEf);
    info->mask = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, MASKf);

    return rv;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_traverse
 * Purpose:
 *      Traverse all valid MPLS entropy label identifier entries 
 *      and call the supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per MPLS ELI entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2p_mpls_entropy_identifier_traverse(
    int unit, 
    bcm_mpls_entropy_identifier_traverse_cb cb, 
    void *user_data)
{
    int rv = BCM_E_NONE;
    uint64 val64;
    int valid;
    bcm_mpls_entropy_identifier_t entropy_info;

    sal_memset(&entropy_info, 0, sizeof(entropy_info));
    BCM_IF_ERROR_RETURN(READ_MPLS_ENTROPY_LABEL_CONFIG_64r(unit,
                &val64));
    valid = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALIDf);
    if (!valid) {
        return BCM_E_EMPTY;
    }

    entropy_info.label = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, VALUEf);
    entropy_info.mask = soc_reg64_field32_get(unit,
            MPLS_ENTROPY_LABEL_CONFIG_64r, val64, MASKf);

    rv = cb(unit, &entropy_info, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
        return rv;
    }
#endif

    return rv;
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* BCM_MPLS_SUPPORT */
#endif /* INCLUDE_L3 */
