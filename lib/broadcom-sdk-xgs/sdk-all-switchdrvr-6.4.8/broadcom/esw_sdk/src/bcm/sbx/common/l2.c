/*
 * $Id: l2.c,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * L2 - Broadcom StrataSwitch Layer-2 switch API.
 */

#include <shared/bsl.h>

#include <soc/cm.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <soc/l2x.h>
#include <bcm/l2.h>

#include <bcm_int/sbx/mbcm.h>

/*
 * Function:
 *     bcm_l2_key_dump
 * Purpose:
 *     Dump the key (VLAN+MAC) portion of a hardware-independent
 *     L2 address for debugging.
 * Parameters:
 *     unit  - Unit number
 *     pfx   - String to print before output
 *     entry - Hardware-independent L2 entry to dump
 *     sfx   - String to print after output
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - Failure, null param
 */
int
bcm_sbx_l2_key_dump(int unit, char *pfx, bcm_l2_addr_t *entry, char *sfx)
{
    if ((pfx == NULL) || (entry == NULL) || (sfx == NULL)) {
        return BCM_E_PARAM;
    }

    LOG_CLI((BSL_META_U(unit,
                        "l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
                        "%02x%02x%02x%s"), pfx, entry->vid,
             entry->mac[0], entry->mac[1], entry->mac[2],
             entry->mac[3], entry->mac[4], entry->mac[5], sfx));

    return BCM_E_NONE;
}
