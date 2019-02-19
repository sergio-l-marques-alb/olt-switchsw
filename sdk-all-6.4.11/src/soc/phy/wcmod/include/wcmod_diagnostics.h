/*
 * $Id: $
 *
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
 */
#include <sal/types.h>
#ifndef _WCMOD_DIAGNOSTICS_H_
#define _WCMOD_DIAGNOSTICS_H_

#include "wcmod_defines.h"
#ifdef _SDK_WCMOD_
#include <sal/types.h>
#endif

/*#define SPEED_CALC_FREQ       156.25 */
/*#define SPEED_CALC_FREQ_UNITS "MHz" */

#define MAX_TAP_SUM_CONSTRAINT 63
#define MIN_TAP_EYE_CONSTRAINT 17

/* #define BER_EXTRAP_SPD_UP */
#define MAX_LOOPS 47
#define MIN_RUNTIME  1
#define INDEX_UNINITIALIZED  (-1)
#define SPD_UP_MAX_RUNTIME 64
#ifdef BER_EXTRAP_SPD_UP
#define MAX_RUNTIME SPD_UP_MAX_RUNTIME
#else
#define MAX_RUNTIME 256
#endif
#define HI_CONFIDENCE_ERR_CNT 100    /* bit errors to determine high confidence */
#define LO_CONFIDENCE_MIN_ERR_CNT 10 /* bit errors, exit condition for low confidence */
#define HI_CONFIDENCE_MIN_ERR_CNT 20 /* bit errors exit condition for high confidence */
#define VEYE_UNIT 1.75
#define HEYE_UNIT 3.125
#define WC_UTIL_MAX_ROUND_DIGITS (8)

/* Do not change the value, used as index */
#define WC_UTIL_VEYE           0  /* vertical eye */
#define WC_UTIL_VEYE_U         1  /* vertical eye up */
#define WC_UTIL_VEYE_D         2  /* vertical eye down */
#define WC_UTIL_HEYE_R         3  /* horizontal right eye */
#define WC_UTIL_HEYE_L         4  /* horizontal left eye */

typedef struct {
    int total_errs[MAX_LOOPS];
    int total_elapsed_time[MAX_LOOPS];
    int mono_flags[MAX_LOOPS];
    int max_loops;
    int offset_max;
    int veye_cnt;
    uint32 rate;      /* frequency in KHZ */
    int first_good_ber_idx;
    int first_small_errcnt_idx;
    int target_ber;    /* this is the target BER example, -18, representing 10^(-18) */
    int tolerance;     /* this is the percentage  specify +/- % opening/margin of the target_ber */
} WCMOD_EYE_DIAG_INFOt;

extern int wcmod_diag_autoneg(wcmod_st *ws);
extern int wcmod_diag_general(wcmod_st *ws);
extern int wcmod_diag_internal_traffic(wcmod_st *ws);
extern int wcmod_diag_link(wcmod_st *ws);
extern int wcmod_diag_topology(wcmod_st *ws);
extern int wcmod_diag_speed(wcmod_st *ws);
extern int _wcmod_eye_margin_data_get(wcmod_st* ws, WCMOD_EYE_DIAG_INFOt *pInfo,int type);
extern int _wcmod_eye_margin_ber_cal(WCMOD_EYE_DIAG_INFOt *pInfo, int type);
extern int _wcmod_eye_margin_diagram_cal(WCMOD_EYE_DIAG_INFOt *pInfo, int type);

#endif

