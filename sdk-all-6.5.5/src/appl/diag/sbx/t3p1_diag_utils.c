/*
 * $Id: t3p1_diag_utils.c,v 1.1 Broadcom SDK $
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
 * File:        t3p1_diag_utils.c
 * Purpose:     Caladan3 diag shell utilities
 * Requires:
 *
 */

#if defined(BCM_CALADAN3_SUPPORT) && defined(BCM_CALADAN3_T3P1_SUPPORT)

#include <shared/bsl.h>

#include <sal/types.h>

#include <soc/shared/mde_agent_msg.h>
#include <netinet/in.h>

#include <soc/sbx/t3p1/t3p1_int.h>
#include <soc/sbx/t3p1/t3p1_defs.h>
#include <soc/sbx/t3p1/t3p1_diags.h>

/*
 * Function:
 *     mde_agent_t3p1_ocm_table_info_get
 * Purpose:
 *     Process a ocm get OCM table info
 * Parameters:
 *     data      - Pointer to message data
 *     length    - Length of message adta in bytes
 * Returns:
 *     BCM_E_NONE    Success
 *     BCM_E_XXX     Failure
 */
int mde_agent_t3p1_ocm_table_info_get(int unit, uint8 *rx_data, uint32 rx_length, uint16 *tx_flags, uint8 **tx_data, uint32 *tx_length) {
    int rv = _SHR_E_NONE;
    uint8  *name;
    uint32 *index;
    uint32 *size;
    uint32 *width;
    uint32 *base;
    uint32 *limit;
    uint32 *segment;
    uint32 *segment_flags;
    uint8  *data;
    int i, j, k;
    uint32 count;
    soc_sbx_t3p1_state_t *fe =(soc_sbx_t3p1_state_t *) SOC_SBX_CONTROL(unit)->drv;
    soc_sbx_t3p1_table_manager_t *tm = fe->tableinfo;
    soc_sbx_t3p1_table_desc_t *td;
    uint32 max_tables = SOC_SBX_T3P1_TABLE_MAX_ID;

    *tx_length = 0;
    *tx_data = NULL;

    /*
     * Data should consist of:
     * uint32 port
     * uint32 address
     * uint32 count
     */
    if (rx_length != 0) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "MDE_AGENT-Socket: Unexpected data length:%d\n"), rx_length));
        rv = _SHR_E_PARAM;
    } else {
        /*
         * Find number of table entries
         */
        count = 0;
        for (i = 0; i < max_tables; i++) {
            td = &tm->tables[i];
            if (td->name != NULL) {
                for (j = 0;j < td->nbanks;j++) {
                    count++;
                }
            }
        }

        *tx_length = sizeof(uint32) + (count * (MDE_AGENT_MAX_TABLE_NAME_LEN + (sizeof(uint32) * 7)));

        /*
         * Allocate buffer memory
         */
        *tx_data = (uint8*)sal_alloc(*tx_length, "ocm table info");
        data = *tx_data;
        *((uint32*)data) = htonl(count);
        data += sizeof(uint32);
        name = (uint8*)data;
        index = (uint32*)(name + (count * MDE_AGENT_MAX_TABLE_NAME_LEN));
        size  = index + count;
        width = size + count;
        base  = width + count;
        limit = base + count;
        segment = limit + count;
        segment_flags = segment + count;

        /*
         * Populate buffer
         */
        k = 0;
        for (i = 0; i < max_tables; i++) {
            td = &tm->tables[i];
            if (td->name != NULL) {
                for (j = 0;j < td->nbanks;j++) {
/*
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit,
                                          "MDE_AGENT: OCM Table entry: Name:%s seg:%d bank:%d\n"),
                               td->name,
                               td->banks[j].segment,
                               td->banks[j].mbank));
*/
                     /* coverity[secure_coding] */
                        strcpy((char*)name, td->name);
                        name += MDE_AGENT_MAX_TABLE_NAME_LEN;
                        index[k] = htonl(j);
                        size[k] = htonl(td->banks[j].size);
                        width[k] = htonl(td->banks[j].width);
                        base[k] = htonl(td->banks[j].base);
                        limit[k] = htonl(td->banks[j].limit);
                        segment[k] = htonl(td->banks[j].segment);
                        segment_flags[k] = htonl(td->banks[j].segflags);
                        k++;
                }
            }
        }

    }
    return rv;
}

#endif /* BCM_CALADAN3_SUPPORT && BCM_CALADAN3_T3P1_SUPPORT */
