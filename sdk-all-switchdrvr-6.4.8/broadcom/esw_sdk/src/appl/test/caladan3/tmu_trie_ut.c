/* $Id: tmu_trie_ut.c,v 1.7 Broadcom SDK $
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
 */
#include <shared/bsl.h>

#include <soc/defs.h>

#if defined(BCM_CALADAN3_SUPPORT)

#include "../c3sw_test.h"
#include <soc/sbx/caladan3/tmu/taps/trie.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <shared/alloc.h>
#include <sal/core/time.h>

extern int tmu_taps_trie_ut(int id, unsigned int seed);
extern int tmu_taps_kshift_ut(void);
extern int tmu_trie_split_ut(unsigned int seed);
extern int tmu_taps_bpm_trie_ut(int id, unsigned int seed);
extern int tmu_taps_util_get_bpm_pfx_ut(void);

/*********************/
/*     Test Runner   */
/*********************/
int c3_ut_tmu_taps_trie_test_init(c3sw_test_info_t *testinfo, void *userdata)
{
    int rv=SOC_E_NONE;
    return rv;
}

int
c3_ut_tmu_taps_trie_test_run(c3sw_test_info_t *testinfo, void *userdata)
{
    int rv=SOC_E_NONE, index=0;

    if (testinfo->testid > 0) {
        if (testinfo->testid < 7) {
            rv = tmu_taps_trie_ut(testinfo->testid, testinfo->nTestSeed);
        } else if (testinfo->testid == 7) {
            rv = tmu_taps_kshift_ut();
        } else if (testinfo->testid == 8) {
            rv = tmu_trie_split_ut(testinfo->nTestSeed);
        } else if (testinfo->testid == 9) {
            rv = tmu_taps_bpm_trie_ut(testinfo->testid, testinfo->nTestSeed);
        } else if (testinfo->testid == 10) {
            rv = tmu_taps_util_get_bpm_pfx_ut();
        } else {
            cli_out("TEST ID: %d not supported !!!!\n", testinfo->testid);
            return SOC_E_PARAM;
        }
    } else { /* run all test */
        for (index=0; index < 7 && SOC_SUCCESS(rv); index++) {
            rv = tmu_taps_trie_ut(index, testinfo->nTestSeed);
        }
        if (SOC_SUCCESS(rv)) {
            rv = tmu_taps_kshift_ut();
        }
        if (SOC_SUCCESS(rv)) {
            rv = tmu_trie_split_ut(testinfo->nTestSeed);
        }
        if (SOC_SUCCESS(rv)) {
            rv = tmu_taps_bpm_trie_ut(9, testinfo->nTestSeed);
        }
        if (SOC_SUCCESS(rv)) {
            rv = tmu_taps_util_get_bpm_pfx_ut();
        }
        if (SOC_FAILURE(rv)) {
            cli_out("\n Unit Tests Failed !!!!!!!!!!\n");
        }
    }

    return rv;
}

int
c3_ut_tmu_taps_trie_test_done(c3sw_test_info_t *testinfo, void *userdata)
{
    int rv=SOC_E_NONE;
    return rv;
}

#endif /* #ifdef BCM_CALADAN3_SUPPORT */
