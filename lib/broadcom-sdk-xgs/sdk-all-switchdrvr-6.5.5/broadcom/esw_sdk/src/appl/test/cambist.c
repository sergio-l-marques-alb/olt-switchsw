/*
 * $Id: l2uc.c,v 1.0 Broadcom SDK $
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
 * TR test for CAMBIST.
 * The test has no CLI parameters. Simply call tr 505 from command line.
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <sal/core/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>

#include "testlist.h"

#ifdef BCM_TOMAHAWK_SUPPORT
#include "cambist.h"
#include "th_cambist.h"

#define CAMBIST_TIMEOUT 100000

typedef struct cambist_test_s {
    cambist_t *cambist_array;
    uint32 total_cambist_count;
    uint32 total_failures;
    uint32 done_bit_mask;
    uint32 bist_success_mask;
    uint32 bad_input;
    uint32 test_fail;
} cambist_test_t;

static cambist_test_t *cambist_test_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      get_cambist_array
 * Purpose:
 *      Populate members of cambist_test_t depending on the chip under test.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
get_cambist_array(int unit)
{
    uint16 dev_id;
    uint8 rev_id;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    soc_cm_get_id (unit, &dev_id, &rev_id);

    if (dev_id == BCM56960_DEVICE_ID) {
        cli_out("\nPopulating memory array for Tomahawk");
        cambist_test_p->cambist_array = th_cambist_array;
        cambist_test_p->total_cambist_count = TH_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = TH_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = TH_BIST_SUCCESS_MASK;
    } else {
        cli_out("\nUnsupported chip");
        cambist_test_p->bad_input = 1;
    }
}

/*
 * Function:
 *      run_cambist
 * Purpose:
 *      Run a CAMBIST for a TCAM.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      cambist_p: Pointer to structure of type cambist_t.
 *
 * Returns:
 *     Nothing
 */

static void
run_cambist(int unit, cambist_t *cambist_p)
{
    int count;
    uint32 bist_en;
    uint32 bist_status;
    uint64 bist_status_64;
    uint32 bist_done;
    uint32 bist_success;
    uint64 zero_64;
    int timeout = CAMBIST_TIMEOUT;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    COMPILER_64_SET(zero_64, 0x0, 0x0);

    if (SOC_REG_IS_VALID(unit, cambist_p->cambist_config)) {
        soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                               BIST_DBG_DATA_SLICE_OR_STATUS_SELf, 0x00);
    }

    if (cambist_p->mode != NO_MODE) {
        soc_reg_field32_modify(unit, cambist_p->cambist_control, REG_PORT_ANY,
                               cambist_p->mode_field, cambist_p->mode);
    }

    for (count = 0; count < cambist_p->cambist_count; count++) {
        cli_out("\nRunning Cambist %s count %0d",
                                    cambist_p->cambist_name, count);
        timeout = CAMBIST_TIMEOUT;
        bist_done = 0;
        bist_success = 0;
        bist_en = 0x1 << count;
        soc_reg_field32_modify(unit, cambist_p->cambist_control, REG_PORT_ANY,
                               cambist_p->cambist_enable, bist_en);

        while (bist_done == 0 && timeout > 0) {
            (void) soc_reg_get(unit, cambist_p->cambist_status, REG_PORT_ANY,
                                                            0, &bist_status_64);

            bist_status = COMPILER_64_LO(bist_status_64);

            bist_done = bist_status & cambist_test_p->done_bit_mask;
            timeout--;
        }

        if (timeout > 0) {
            bist_success = bist_status & cambist_test_p->bist_success_mask;

            if (bist_success != 0) {
                cli_out("\nCambist %s count %0d passed",
                                    cambist_p->cambist_name, count);
            } else {
                test_error(unit, "\n*ERROR: Cambist %s count %0d FAILED",
                                            cambist_p->cambist_name, count);
                cambist_test_p->test_fail = 1;
                cambist_test_p->total_failures++;
            }
        } else {
            test_error(unit, "\n*ERROR: Cambist %s count %0d TIMED OUT",
                                            cambist_p->cambist_name, count);
            cambist_test_p->test_fail = 1;
            cambist_test_p->total_failures++;
        }

        soc_reg_field32_modify(unit, cambist_p->cambist_control, REG_PORT_ANY,
                               cambist_p->cambist_enable, 0x0);

        (void) soc_reg_set(unit, cambist_p->cambist_status,
                           REG_PORT_ANY, 0, zero_64);
    }
}

/*
 * Function:
 *      run_cambist
 * Purpose:
 *      Run a CAMBIST for a TCAM.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      cambist_p: Pointer to structure of type cambist_t.
 *
 * Returns:
 *     Nothing
 */

static void
run_all_cambists(int unit)
{
    int i;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    for (i = 0; i < cambist_test_p->total_cambist_count; i++) {
        run_cambist(unit, &cambist_test_p->cambist_array[i]);
    }
}

int
cambist_test_init(int unit, args_t *a, void **pa)
{
    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    cli_out("\nCalling cambist_test_init");

    cambist_test_p = sal_alloc(sizeof(cambist_test_t), "cambist_test");
    sal_memset(cambist_test_p, 0, sizeof(cambist_test_t));
    cambist_test_parray[unit] = cambist_test_p;

    cambist_test_p->bad_input = 0;
    cambist_test_p->test_fail = 0;
    cambist_test_p->total_failures = 0;

    get_cambist_array(unit);

    return 0;
}

int
cambist_test(int unit, args_t *a, void *pa)
{
    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    cli_out("\nCalling cambist_test");

    (void) soc_mem_scan_stop(unit);
    (void) bcm_linkscan_enable_set(unit, 0);
    soc_counter_stop(unit);

    if (cambist_test_p->bad_input == 1) {
        goto done;
    }

    run_all_cambists(unit);

done:

    return 0;
}

int
cambist_test_done(int unit, void *pa)
{
    int rv;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    cli_out("\nCalling cambist_test_done");

    if (cambist_test_p->bad_input == 1) {
        goto done;
    }

    cli_out("\n");
    cli_out("\nTotal CAMBISTs run = %0d", cambist_test_p->total_cambist_count);
    cli_out("\nTotal failures = %0d", cambist_test_p->total_failures);

done:

    if (cambist_test_p->bad_input == 1) {
        cambist_test_p->test_fail = 1;
    }

    if (cambist_test_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    } else {
        rv = BCM_E_NONE;
    }

    if (cambist_test_p->test_fail == 0) {
        cli_out("\n********* All CAMBIST tests passed *********");
    } else {
        test_error(unit, "********** ERROR: CAMBIST test failed *********");
    }
    cli_out("\n");

    sal_free(cambist_test_p);
    return rv;
}

#endif /* BCM_TOMAHAWK_SUPPORT */
