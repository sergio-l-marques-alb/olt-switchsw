/*
 * $Id: counters.c,v 1.93 Broadcom SDK $
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
 * Module: internal counter processor work
 *
 * Purpose:
 *     Access to counters hosted by Soc_petra counter processors
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_CNT

#include <shared/bsl.h>

#include <soc/dpp/drv.h>

#include "soc/drv.h"
#include "soc/dpp/mbcm.h"
#include "bcm_int/dpp/error.h"
#include "bcm_int/dpp/utils.h"
#include "bcm_int/dpp/counters.h"
#include "bcm_int/common/debug.h"

#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/wb_db_counters.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#ifdef  BCM_ARAD_SUPPORT   
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/port_sw_db.h>
#endif



/****************************************************************************
 *
 *  Internal implementation
 */

/*
 *  With the _SATURATE_INSTEAD_OF_OVERFLOW option TRUE, counters will saturate
 *  (all bits one) instead of overflow.  With it false, counters will overflow.
 *  The overflow case is a bit faster and slightly improves coherency (not to
 *  mention reduces stack depth by a couple octbytes here and there).
 *
 *  Note that emulated counters also honor this setting -- if saturation, and
 *  the components of an emulated counter would be too big, the emulated
 *  counter will be saturated.  Otherwise it will overflow.
 */
#define _SATURATE_INSTEAD_OF_OVERFLOW TRUE

/*
 *  With the _TRACK_DIAGS_IN_WARMBOOT_ARENA option TRUE, the counter
 *  diagnostics will be kept up to date in the warmboot backing store.  This is
 *  rather expensive, since there are a lot of updates by the background thread
 *  and by directed access (the background thread alone updates numerous
 *  statistics and can run as many as 100 times per second in the default
 *  configuration).
 */
#define _TRACK_DIAGS_IN_WARMBOOT_SPACE FALSE

/*
 *  _BACKGROUND_THREAD_ITER_MIN/MAX are the limits on the automatic delay
 *  adjustment that will be used to try to keep the background counter thread
 *  processor use reasonable, while avoiding over-running the FIFO.
 *
 *  _BACKGROUND_THREAD_ITER_DEFAULT is where the background thread starts.  It
 *  will increase the delay as long as the delay is below MAX and the deepest
 *  FIFO during the iteration was below _FIFO_DEPTH_MIN.  It will decrease the
 *  delay as long as the delay is above MIN and the deepest FIFO during the
 *  iteration was above _FIFO_DEPTH_PREF.  DEFAULT should generally be equal to
 *  MAX, since it will otherwise quickly settle to MAX during startup.
 *
 *  _BACKGROUND_THREAD_ITER_ADJUST is the amount of adjustment that will be
 *  applied when making adjustments, with the exception that if the background
 *  thread ever sees a read go above the _FIFO_DEPTH_MAX, and the current value
 *  is greater than MIN, it will bet set to MIN.
 *
 *  These are all expressed in microseconds, but note that different operating
 *  systems have different timer resolution, and the accuracy is guaranteed to
 *  be no tighter than half a timer tic.  Also note that this is delay between
 *  the end of one poll and the beginning of the next -- it is not measured
 *  between the same point on adjacent polls.
 */
#define _FIFO_DEPTH_MIN   2
#define _FIFO_DEPTH_MAX(unit)   (SOC_DPP_DEFS_GET(unit, counter_fifo_depth) - 1)
#define _FIFO_DEPTH_PREF(unit)  ((_FIFO_DEPTH_MIN + _FIFO_DEPTH_MAX(unit)) / 2)
#define  _BCM_COUNTER_THREAD_WAIT_TIMEOUT    1000 
/* Maximum delay access: 100 ms in Petra-B, 1 sec in Arad. Possible enlargement in Arad to up to 3 minutes  */
#define _BACKGROUND_THREAD_ITER_MIN      10000
#define _BACKGROUND_THREAD_ITER_MAX(unit)     (SOC_IS_ARAD(unit)? (_BACKGROUND_THREAD_ITER_MIN * 100): (_BACKGROUND_THREAD_ITER_MIN * 10))
#define _BACKGROUND_THREAD_ITER_DEFAULT(unit) (_BACKGROUND_THREAD_ITER_MAX(unit))
/* Reach min <-> max in 20 steps max */
#define _BACKGROUND_THREAD_ITER_ADJUST(unit)    ((_BACKGROUND_THREAD_ITER_MAX(unit) + _BACKGROUND_THREAD_ITER_MIN) / 20) 

/*
 * BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if defined (BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
#define BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(function_call,err_string,is_err_msg)   \
    rv = function_call;                                                                  \
    if(rv != BCM_E_NONE) {                                                               \
        if (is_err_msg) {                                                                \
            LOG_ERROR(BSL_LS_BCM_CNT, \
                      (BSL_META(err_string)));           			\
        }                                                                                \
        else{                                                                            \
            BCMDNX_IF_ERR_EXIT(rv);                                                 \
        }                                                                                \
    }
#else /*defined (BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)*/
#define BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(function_call,err_string,is_err_msg)
#endif /*defined (BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)*/
#else /* BCM_WARM_BOOT_SUPPORT */
#define BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(function_call,err_string,is_err_msg)
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT
 */
#if defined(BCM_WARM_BOOT_SUPPORT) && (_TRACK_DIAGS_IN_WARMBOOT_SPACE || defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE))
#define BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT \
    int rv = BCM_E_NONE;
#else /* defined(BCM_WARM_BOOT_SUPPORT) && (_TRACK_DIAGS_IN_WARMBOOT_SPACE || defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)) */
#define BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT
#endif /* defined(BCM_WARM_BOOT_SUPPORT) && (_TRACK_DIAGS_IN_WARMBOOT_SPACE || defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)) */


/* check if one of the new offset based counters */
#define IS_OFFSET_BASED_COUNTER(counter)  (((counter) >= bcm_dpp_counter_offset0_pkts) && ((counter) <= bcm_dpp_counter_offset7_bytes))
#define OFFSET_BASED_COUNTER_OFFSET_GET(counter)  (((counter) - bcm_dpp_counter_offset0_pkts)/2)

/* check lsb/msb counter*/
#define IS_STR_LSB0(propval, string) ((sal_strcmp(propval, (string)) == 0) || (sal_strcmp(propval, (string "_0")) == 0) || (sal_strcmp(propval, (string "_LSB_0")) == 0)  || (sal_strcmp(propval, (string "_LSB")) == 0))
#define IS_STR_LSB1(propval, string) ((sal_strcmp(propval, (string "_1")) == 0) || (sal_strcmp(propval, (string "_LSB_1")) == 0))
#define IS_STR_MSB0(propval, string) ((sal_strcmp(propval, (string "_MSB_0")) == 0) || (sal_strcmp(propval, (string "_MSB"))== 0))
#define IS_STR_MSB1(propval, string) ((sal_strcmp(propval, (string "_MSB_1")) == 0))



/*define for each counter(except the offset based counters) in bcm_dpp_counter_t what is it count: 
  e.g fwd_all = fwd_green, fwd_yellow, fwd_red, fwd_black*/
uint32 counters_def[bcm_dpp_counter_count] = {0};




/*
 *  The hardware contains multiple counter processors; each one contains a
 *  number of counter pairs.  Unhappily, the hardware counter pairs only
 *  contain a 32b byte counter and a 25b frame counter.  These both are rather
 *  too small for the device, based upon its maximum bandwidth, and so there
 *  is a mechanism that places counters that are 'nearly ready to overflow'
 *  into a FIFO, which must be cleared regularly (at least 10Hz[?]).
 *
 *  While just reading the FIFO is enough to maintain things, it is not enough
 *  when the application needs to read or write counters.  There are several
 *  situations where the mechanism is not sufficient, but most of them relate
 *  to situations where data rate is not near the maximum:
 *
 *  * Since the counter 'algorithmic' mechanism only ejects counters that are
 *    near to full, low-rate counters will have long latencies between updates,
 *    and the update granularity will not reflect the actual rate except over
 *    extremely long intervals.
 *
 *  In order to get counters to be as accurate as possible, we want to ensure a
 *  counter that is about to be read is updated, and that a counter that is
 *  about to be written does not have residue values, either in the hardware
 *  counter buffers or in the FIFO.  In either location it could have up to
 *  2^25-1 frames and 2^32-1 bytes -- no small discrepancy.
 *
 *  We don't want to block the background task on semaphore, nor do we want
 *  every counter access to touch a semaphore; mostly this is because of the
 *  overhead of dealing with them on many operating systems.
 *
 *  Instead of mucking about with semaphores, there is a simple locking
 *  mechanism designed into the local cache tables.  The background thread sets
 *  a flag (background_active) when it is updating the cache, and clears it
 *  when it is done.  If another flag, background_defer, is set, it will skip
 *  the current cycle.  When accessing counters on behalf of an API caller, we
 *  set the background_defer flag and then wait for the background_active flag
 *  to become zero.  Once this is done, we clear the FIFO and update the
 *  counter(s) that were requested, and then set background_defer to zero.
 */

/*
 *  Names for the various statistics
 */
CONST char* CONST bcm_dpp_counter_t_names[bcm_dpp_counter_count + 1] = {
    /* forwarded and dropped frames and bytes */
    "packets",
    "bytes",
    "green packets",
    "green bytes",
    "not_green packets",
    "not_green bytes",
    "yellow packets",
    "yellow bytes",
    "not_yellow packets",
    "not_yellow bytes",
    "red packets",
    "red bytes",
    "not_red packets",
    "not_red bytes",
    /* forwarded frames and bytes */
    "forwarded packets",
    "forwarded bytes",
    "forwarded green packets",
    "forwarded green bytes",
    "forwarded not_green packets",
    "forwarded not_green bytes",
    "forwarded yellow packets",
    "forwarded yellow bytes",
    "forwarded not_yellow packets",
    "forwarded not_yellow bytes",
    "forwarded red packets",
    "forwarded red bytes",
    "forwarded not_red packets",
    "forwarded not_red bytes",
    /* discarded frames and bytes */
    "dropped packets",
    "dropped bytes",
    "dropped green packets",
    "dropped green bytes",
    "dropped not_green packets",
    "dropped not_green bytes",
    "dropped yellow packets",
    "dropped yellow bytes",
    "dropped not_yellow packets",
    "dropped not_yellow bytes",
    "dropped red packets",
    "dropped red bytes",
    "dropped not_red packets",
    "dropped not_red bytes",
    /* entry by index */
    "offset0 packets",
    "offset0 bytes",
    "offset1 packets",
    "offset1 bytes",
    "offset2 packets",
    "offset2_bytes",
    "offset3 packets",
    "offset3 bytes",
    "offset4 packets",
    "offset4 bytes",
    "offset5 packets",
    "offset5 bytes",
    "offset6 packets",
    "offset6 bytes",
    "offset7 packets",
    "offset7 bytes",
    /* limit */
    "unknown statistic" /* NOT A VALID ENTRY; MUST BE LAST */
};

/*
 *  Handle the counter state for multiple units.
 */
_bcm_dpp_counter_state_t *_bcm_dpp_counter_state[SOC_MAX_NUM_DEVICES] = {NULL};
int _bcm_counter_thread_is_running[SOC_MAX_NUM_DEVICES];
/*
 *  Various common bits
 */
#define _COUNTER_UNIT_CHECK(_u,_ud) \
    if ((0 > (_u)) || (SOC_MAX_NUM_DEVICES <= (_u))) { \
        LOG_ERROR(BSL_LS_BCM_CNT, \
                  (BSL_META_U(unit, \
                              "unit %d is not valid\n"), \
                   _u)); \
        return BCM_E_UNIT; \
    } \
    if (NULL == ((_ud) = _bcm_dpp_counter_state[_u])) { \
        LOG_ERROR(BSL_LS_BCM_CNT, \
                  (BSL_META_U(unit, \
                              "unit %d does not have DPP counters" \
                              " inited\n"), \
                   _u)); \
        return BCM_E_INIT; \
    }

/*
 *  Offsets into the cache based upon the color and mode
 */
#define _OFS_E_CN 0
#define _OFS_E_CY_GREEN 0
#define _OFS_E_CY_YELLOW _SUB_COUNT

#define _OFS_COUNT (_SUB_COUNT * 5)

/*static _bcm_cnt_dma_fifo_ctrl_t *_bcm_cnt_dma_ctrl[BCM_MAX_NUM_UNITS];*/

/*
 *   Name
 *     _bcm_dpp_counters_per_set
 *   Purpose
 *     Get the number of counters in a set
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = which counter processor
 *     OUT unsigned int *counters = where to put counters per set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
STATIC int
_bcm_dpp_counters_per_set(_bcm_dpp_counter_state_t *unitData,
                          unsigned int proc,
                          unsigned int *counters)
{
    int unit = unitData->unit;
    BCMDNX_INIT_FUNC_DEFS;
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d invalid counter"
                                           "processor ID %u"),
                                           unit,
                                           proc));
    }
    switch(unitData->proc[proc].mode.mode_statistics){
	case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR:
	case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR:
	case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR:
        *counters = 1;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR:
        *counters =  2;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI:
        *counters = 2;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW:
        *counters =  4;
        break;

    case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI:
        *counters =  5;
        break;
#ifdef BCM_88660_A0
    case SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR:
    case SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR: /*slide*/
        if(SOC_IS_ARAD_B1_AND_BELOW(unit)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d invalid counter mode %d"),
             unit,
             unitData->proc[proc].mode.mode_statistics));
        }
        *counters = 2;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS:
        if(SOC_IS_ARAD_B1_AND_BELOW(unit)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d invalid counter mode %d"),
             unit,
             unitData->proc[proc].mode.mode_statistics));
        }
        *counters = unitData->proc[proc].mode.custom_mode_params.nof_counters;
        break;
#endif
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d invalid counter mode %d"),
                     unit,
                     unitData->proc[proc].mode.mode_statistics));
    } /* switch (unitData->proc[proc].format) */
    if ((SOC_IS_ARADPLUS(unit)) && unitData->proc[proc].mode.format == ARAD_CNT_FORMAT_PKTS_AND_PKTS){
        *counters =  *counters / 2;
    }
    BCM_EXIT;
exit:
      BCMDNX_FUNC_RETURN;
}


void _bcm_dpp_counters_def_init(void){
    counters_def[bcm_dpp_counter_pkts]              = counters_def[bcm_dpp_counter_bytes]                 = GREEN_FWD | GREEN_DROP | YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP | BLACK_FWD | BLACK_DROP;
    counters_def[bcm_dpp_counter_green_pkts]        = counters_def[bcm_dpp_counter_green_bytes]           = GREEN_FWD | GREEN_DROP;
    counters_def[bcm_dpp_counter_not_green_pkts]    = counters_def[bcm_dpp_counter_not_green_bytes]       = YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP | BLACK_FWD | BLACK_DROP ;
    counters_def[bcm_dpp_counter_yellow_pkts]       = counters_def[bcm_dpp_counter_yellow_bytes]          = YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP;
    counters_def[bcm_dpp_counter_not_yellow_pkts]   = counters_def[bcm_dpp_counter_not_yellow_bytes]      = GREEN_FWD | GREEN_DROP | BLACK_FWD | BLACK_DROP;
    counters_def[bcm_dpp_counter_red_pkts]          = counters_def[bcm_dpp_counter_red_bytes]             = BLACK_FWD | BLACK_DROP;
    counters_def[bcm_dpp_counter_not_red_pkts]      = counters_def[bcm_dpp_counter_not_red_bytes]         = GREEN_FWD | GREEN_DROP | YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP;
    counters_def[bcm_dpp_counter_fwd_pkts]           = counters_def[bcm_dpp_counter_fwd_bytes]            = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_green_pkts]     = counters_def[bcm_dpp_counter_fwd_green_bytes]      = GREEN_FWD;
    counters_def[bcm_dpp_counter_fwd_not_green_pkts] = counters_def[bcm_dpp_counter_fwd_not_green_bytes]  = YELLOW_FWD | RED_FWD | BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_yellow_pkts]    = counters_def[bcm_dpp_counter_fwd_yellow_bytes]     = YELLOW_FWD | RED_FWD;
    counters_def[bcm_dpp_counter_fwd_not_yellow_pkts]= counters_def[bcm_dpp_counter_fwd_not_yellow_bytes] = GREEN_FWD | BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_red_pkts]       = counters_def[bcm_dpp_counter_fwd_red_bytes]        = BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_not_red_pkts]   = counters_def[bcm_dpp_counter_fwd_not_red_bytes]    = GREEN_FWD | YELLOW_FWD | RED_FWD;
    counters_def[bcm_dpp_counter_drop_pkts]          = counters_def[bcm_dpp_counter_drop_bytes]           = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_green_pkts]    = counters_def[bcm_dpp_counter_drop_green_bytes]     = GREEN_DROP;
    counters_def[bcm_dpp_counter_drop_not_green_pkts]= counters_def[bcm_dpp_counter_drop_not_green_bytes] = YELLOW_DROP | RED_DROP | BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_yellow_pkts]   = counters_def[bcm_dpp_counter_drop_yellow_bytes]    = YELLOW_DROP | RED_DROP;
    counters_def[bcm_dpp_counter_drop_not_yellow_pkts]= counters_def[bcm_dpp_counter_drop_not_yellow_bytes]= GREEN_DROP | BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_red_pkts]      = counters_def[bcm_dpp_counter_drop_red_bytes]       = BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_not_red_pkts]  = counters_def[bcm_dpp_counter_drop_not_red_bytes]   = GREEN_DROP | YELLOW_DROP | RED_DROP;
}


int
_bcm_dpp_counters_is_byte_format_get(bcm_dpp_counter_t counter, uint8 *is_byte_format){
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    switch (counter){
    case bcm_dpp_counter_pkts:
    case bcm_dpp_counter_green_pkts:
    case bcm_dpp_counter_not_green_pkts:
    case bcm_dpp_counter_yellow_pkts:
    case bcm_dpp_counter_not_yellow_pkts:
    case bcm_dpp_counter_red_pkts:
    case bcm_dpp_counter_not_red_pkts:
    case bcm_dpp_counter_fwd_pkts:
    case bcm_dpp_counter_fwd_green_pkts:
    case bcm_dpp_counter_fwd_not_green_pkts:
    case bcm_dpp_counter_fwd_yellow_pkts:
    case bcm_dpp_counter_fwd_not_yellow_pkts:
    case bcm_dpp_counter_fwd_red_pkts:
    case bcm_dpp_counter_fwd_not_red_pkts:
    case bcm_dpp_counter_drop_pkts:
    case bcm_dpp_counter_drop_green_pkts:
    case bcm_dpp_counter_drop_not_green_pkts:
    case bcm_dpp_counter_drop_yellow_pkts:
    case bcm_dpp_counter_drop_not_yellow_pkts:
    case bcm_dpp_counter_drop_red_pkts:
    case bcm_dpp_counter_drop_not_red_pkts:
    case bcm_dpp_counter_offset0_pkts:
    case bcm_dpp_counter_offset1_pkts:
    case bcm_dpp_counter_offset2_pkts:
    case bcm_dpp_counter_offset3_pkts:
    case bcm_dpp_counter_offset4_pkts:
    case bcm_dpp_counter_offset5_pkts:
    case bcm_dpp_counter_offset6_pkts:
    case bcm_dpp_counter_offset7_pkts:
        *is_byte_format = FALSE;
        break;
    case bcm_dpp_counter_bytes:
    case bcm_dpp_counter_green_bytes:
    case bcm_dpp_counter_not_green_bytes:
    case bcm_dpp_counter_yellow_bytes:
    case bcm_dpp_counter_not_yellow_bytes:
    case bcm_dpp_counter_red_bytes:
    case bcm_dpp_counter_not_red_bytes:
    case bcm_dpp_counter_fwd_bytes:
    case bcm_dpp_counter_fwd_green_bytes:
    case bcm_dpp_counter_fwd_not_green_bytes:
    case bcm_dpp_counter_fwd_yellow_bytes:
    case bcm_dpp_counter_fwd_not_yellow_bytes:
    case bcm_dpp_counter_fwd_red_bytes:
    case bcm_dpp_counter_fwd_not_red_bytes:
    case bcm_dpp_counter_drop_bytes:
    case bcm_dpp_counter_drop_green_bytes:
    case bcm_dpp_counter_drop_not_green_bytes:
    case bcm_dpp_counter_drop_yellow_bytes:
    case bcm_dpp_counter_drop_not_yellow_bytes:
    case bcm_dpp_counter_drop_red_bytes:
    case bcm_dpp_counter_drop_not_red_bytes:
    case bcm_dpp_counter_offset0_bytes:
    case bcm_dpp_counter_offset1_bytes:
    case bcm_dpp_counter_offset2_bytes:
    case bcm_dpp_counter_offset3_bytes:
    case bcm_dpp_counter_offset4_bytes:
    case bcm_dpp_counter_offset5_bytes:
    case bcm_dpp_counter_offset6_bytes:
    case bcm_dpp_counter_offset7_bytes:
        *is_byte_format = TRUE;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG_NO_UNIT("format %d not supported "), counter));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Name
 *     _bcm_dpp_counters_calc_unavail_formats
 *   Purpose
 *     get list of unavailable formats.
 *   Arguments
 *     IN nof_entries the number of counters
 *     IN entries_bmaps - bmap by color and drop/fwd for each counter
 *     OUT unavail_counters - the counters we cant count
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes:
 *   We assume that each dp_fwd/drop counter can be counted in one entry.
 *   From that assumption we define conflict counters:
 *   counter that contains part of the counter definition but also contains bits that are not part of it
 */
void
_bcm_dpp_counters_calc_unavail_formats(uint32 nof_entries, uint32 *entries_bmaps, bcm_dpp_counter_set_t unavail_counters){
    uint32 i, j, counter_entry[1], do_not_count_entry[1], tmp_counter_bmap = 0;
    
    SHR_BITCLR_RANGE(unavail_counters, 0, bcm_dpp_counter_count);
    /*invalid offset based counters*/
    for(i = 0 ; i < bcm_dpp_counter_count; i++){
        if((IS_OFFSET_BASED_COUNTER(i))&& (OFFSET_BASED_COUNTER_OFFSET_GET(i) >= nof_entries)){
            SHR_BITSET(unavail_counters, i);
        }
    }
    /*look for conflicts*/
    for(i = 0 ; i < nof_entries ; i++){
		if(entries_bmaps[i] == 0){ /*possible just in configurable mode*/
			continue;
		}
		*counter_entry = entries_bmaps[i];
		for(j = 0 ; j < bcm_dpp_counter_count; j++){
            if((IS_OFFSET_BASED_COUNTER(j)) || (*counter_entry == counters_def[j])){
                continue;
            }
            SHR_BITNEGATE_RANGE(&counters_def[j], 0, SOC_TMC_CNT_BMAP_OFFSET_COUNT, &tmp_counter_bmap);
			/*check if there is conflict between the entry and the counter definition. see the function notes*/
			if(((*counter_entry & counters_def[j]) != 0)&& ((*counter_entry & tmp_counter_bmap) != 0)){
				SHR_BITSET(unavail_counters, j);
			}
		}
	}
    /*remove counters that we don't count at all*/
    *counter_entry = 0;
    for(i = 0 ; i < nof_entries ; i++){
		*counter_entry |= entries_bmaps[i];
    }
    SHR_BITNEGATE_RANGE(counter_entry, 0, SOC_TMC_CNT_BMAP_OFFSET_COUNT, do_not_count_entry);
	for(i = 0 ; i < bcm_dpp_counter_count; i++){
		if((!IS_OFFSET_BASED_COUNTER(i)) && ((counters_def[i] & *do_not_count_entry) != 0)){
			SHR_BITSET(unavail_counters, i);
		}
	}
}


/*
 *   Name
 *     _bcm_dpp_counters_calc_formats
 *   Purpose
 *     calc the native and the available counters without considering the counting format mode(Packets , Bytes, etc...)
 *   Arguments
 *     IN nof_entries the number of counters
 *     IN entries_bmaps - bmap by color and drop/fwd for each counter
 *     OUT native_formats - bmap that the i bit represent whether the i counter is native or not
 *     OUT avail_offsets - array if the value is not UNSUPPORTED_COUNTER_OFFSET. its an available counter and we will use the value
 *     to read/write the counter value to the processor counters array.
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *   Doesn't support packets and packets because the entries bmaps are not defined well for this format
 */
int
_bcm_dpp_counters_calc_formats(uint32 nof_entries, uint32 *entries_bmaps,  bcm_dpp_counter_set_t native_formats, uint32 *avail_offsets)
{
    uint32 i, j;
    bcm_dpp_counter_set_t unavail_formats;
    uint8 is_bytes_format;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    _bcm_dpp_counters_calc_unavail_formats(nof_entries, entries_bmaps, unavail_formats);
    /*calc the offsets*/
    for(j = 0; j < bcm_dpp_counter_count ; j++){
        if(SHR_BITGET(unavail_formats, j)){
            continue;
        }
        rv = _bcm_dpp_counters_is_byte_format_get(j, &is_bytes_format);
        BCMDNX_IF_ERR_EXIT(rv);
        avail_offsets[j] = 0;
        for(i = 0 ; i < nof_entries; i++){
            /*native format*/
            if(entries_bmaps[i] == counters_def[j] || ((IS_OFFSET_BASED_COUNTER(j))&& (OFFSET_BASED_COUNTER_OFFSET_GET(j) == i))){
                if(is_bytes_format){
                    avail_offsets[j] = i*_SUB_COUNT + _SUB_BYTES;
                }
                else{
                    avail_offsets[j] = i*_SUB_COUNT + _SUB_PKTS;
                }
                SHR_BITSET(native_formats, j);
                continue;
            }
            /*if the entry is not part of the counter*/
            /* j is limited to bcm_dpp_counter_count - 1 according to the for look. The nof entries of counters_def is counters_def - no overrun*/
            /* coverity[overrun-local:FALSE] */
            if((entries_bmaps[i] & counters_def[j]) == 0){
                continue;
            }
            /*avail format- we use bmap*/
            if(is_bytes_format){
                 SHR_BITSET(&avail_offsets[j], (_SUB_COUNT*i + _SUB_BYTES));
            } else {
                 SHR_BITSET(&avail_offsets[j], (_SUB_COUNT*i + _SUB_PKTS));
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC
int
_bcm_dpp_counter_get_counter_src_pointer(SOC_TMC_CNT_SRC_TYPE src, int src_core, SOC_TMC_CNT_SRC_TYPE *src_ptr){
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    if (src == SOC_TMC_CNT_SRC_TYPE_EPNI_B){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_EPNI_A;
    } else if (src == SOC_TMC_CNT_SRC_TYPE_EPNI_D){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_EPNI_C;
    } else if (src == SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
    } else if (src == SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    } else if (src == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB;
    } else if (src == SOC_TMC_CNT_SRC_TYPE_OAM_ING_B){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_OAM_ING_A;
    } else if (src == SOC_TMC_CNT_SRC_TYPE_OAM_ING_D){
        *src_ptr = SOC_TMC_CNT_SRC_TYPE_OAM_ING_C;
    }
    else{
        *src_ptr = src;
    }
    BCMDNX_FUNC_RETURN;  
}

/*
 *   Name
 *     _bcm_dpp_counter_fifo_process
 *   Purpose
 *     Process the 'algorithmic' FIFO to adjust the cache accordingly
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     OUT unsigned int *depth = where to put greatest FIFO depth this pass
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is called by both the background task and the demand access
 *     functions, so it does not itself handle the locking protocol.
 *
 *     Because the calls to soc_pb_cnt_algorithmic_read from the background process
 *     and API caller may collide, or may collide with the direct read, we
 *     don't do any background updates while performing updates in preparation
 *     for foreground access. This means that in both cases, we need to run the
 *     FIFOs on all of the counter processors.
 */

STATIC int
_bcm_dpp_counter_fifo_process(_bcm_dpp_counter_state_t *unitData,
                              unsigned int proc_mask,
                              unsigned int *depth)
{
    uint32 soc_sandResult;
    unsigned int index;
    unsigned int ofs;
    unsigned int proc;
    unsigned int ctrs = 0;
    SOC_TMC_CNT_RESULT_ARR *data_p = NULL;
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    int fifo_dma_index = 0, fifo_dma_index_old = -1, fifo_dma_offset;
    uint32 engine_id, counter_id;
    int nof_procs; /* processor descriptor */
    SOC_TMC_CNT_COUNTERS_INFO *counter_info_array = NULL;

#if _SATURATE_INSTEAD_OF_OVERFLOW
    uint64 oldval;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
    uint64 *counters;
    _bcm_dpp_counter_proc_info_t *info, *info_array;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT;

    nof_procs = (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)); /* processor descriptor */
    BCMDNX_ALLOC(counter_info_array, (sizeof(SOC_TMC_CNT_COUNTERS_INFO) * nof_procs), "DPP prcc counter info array");
    if (!counter_info_array) {
        BCMDNX_IF_ERR_EXIT(BCM_E_MEMORY);
    }
    /*build info array*/
    sal_memset(counter_info_array, 0x0, sizeof(*counter_info_array) * nof_procs);
    for (proc = 0; proc < unitData->num_counter_procs && proc < nof_procs; proc++) {
        sal_memcpy(&(counter_info_array[proc]), &(unitData->proc[proc].mode), sizeof(unitData->proc[proc].mode));
    }

    data_p = &(unitData->fifo_results);
    for (proc = 0; proc < unitData->num_counter_procs; proc++) {
       if(0 == (proc_mask & (1 << proc))) {
          /* if not in the mask skip*/
          continue;
       }
       info = &(unitData->proc[proc]);
       info_array = unitData->proc;

       /* if wide mode do not collect from the DMA*/
       if ((info->mode.format == SOC_TMC_CNT_FORMAT_PKTS) || (info->mode.format == SOC_TMC_CNT_FORMAT_BYTES) || (info->mode.format == SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE) || (info->mode.format == SOC_TMC_CNT_FORMAT_IPT_LATENCY)) {
           continue;
       }
       SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, info->proc_id, &fifo_dma_index)));     
       if (fifo_dma_index == fifo_dma_index_old) {
           /*allready raad this procs data*/
           continue;
       } 
       fifo_dma_index_old = fifo_dma_index;
       SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_fifo_dma_offset_in_engine, (unit, info->proc_id, &fifo_dma_offset)));


        /* prep */
       SOC_TMC_CNT_RESULT_ARR_clear(data_p);
       /* get the data from the algorithmic read FIFO */
       soc_sandResult = (MBCM_DPP_DRIVER_CALL((unit), mbcm_dpp_cnt_algorithmic_read,
           (unit, fifo_dma_index, counter_info_array, data_p)));
       result = handle_sand_result(soc_sandResult);

        if (result != BCM_E_NONE) {
            /* accounting -- read pass failed */
            info->fifo_read_fails++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_counters_update_fifo_read_fails_state(unit, proc);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            BCMDNX_IF_ERR_EXIT(result);
        } /* if (BCM_E_NONE != tmpRes) */

        /* accounting -- read pass successful */
        info->fifo_read_passes++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_fifo_read_passes_state(unit, proc);
        BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            /* accounting -- total counter reads through FIFO */
        
        /* accounting -- largest single FIFO read set */
        if (data_p->nof_counters > info->fifo_read_max) {
            info->fifo_read_max = data_p->nof_counters;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_counters_update_fifo_read_max_state(unit, proc);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
        }
            /* accounting -- largest FIFO read set this time */
        if (data_p->nof_counters > ctrs) {
            ctrs = data_p->nof_counters;
        }
            /* while there are counters to update, update them */
        for (index = 0; index < data_p->nof_counters; index++) {
            counter_id = data_p->cnt_result[index].counter_id;
            engine_id = data_p->cnt_result[index].engine_id;

            ofs = counter_id * _SUB_COUNT;
            info_array[engine_id].fifo_read_last = counter_id;
            counters = info_array[engine_id].counter;

            info_array[engine_id].fifo_read_items++;
            info_array[engine_id].cache_updates++;
#if _SATURATE_INSTEAD_OF_OVERFLOW
            oldval = counters[ofs + _SUB_PKTS];
            COMPILER_64_ADD_64(counters[ofs + _SUB_PKTS], data_p->cnt_result[index].pkt_cnt);
            if (COMPILER_64_LT(counters[ofs + _SUB_PKTS], oldval)) {
                /* dont allow wrap; saturate instead */
                COMPILER_64_ALLONES(counters[ofs + _SUB_PKTS]);
            }
            oldval = counters[ofs + _SUB_BYTES];
            COMPILER_64_ADD_64(counters[ofs + _SUB_BYTES], data_p->cnt_result[index].byte_cnt);
            if (COMPILER_64_LT(counters[ofs + _SUB_BYTES], oldval)) {
                /* dont allow wrap; saturate instead */
                COMPILER_64_ALLONES(counters[ofs + _SUB_BYTES]);
            }
#else /* _SATURATE_INSTEAD_OF_OVERFLOW */
            COMPILER_64_ADD_64(counters[ofs + _SUB_PKTS], data_p->cnt_result[index].pkt_cnt);
            COMPILER_64_ADD_64(counters[ofs + _SUB_BYTES],data_p->cnt_result[index].byte_cnt);
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
            BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
                _bcm_dpp_wb_counters_update_counter_state(unit, engine_id, ofs + _SUB_PKTS),
                (COUNTER_MSG1("unit %d proc %d error updating warm boot data: counter index %d  \n"), unit, engine_id, ofs + _SUB_PKTS), 1);
            BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
                _bcm_dpp_wb_counters_update_counter_state(unit, engine_id, ofs + _SUB_BYTES),
                (COUNTER_MSG1("unit %d proc %d error updating warm boot data: counter index %d  \n"), unit, engine_id, ofs + _SUB_BYTES), 1);
        }

#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_fifo_read_items_state(unit, proc);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_wb_counters_update_fifo_read_last_state(unit, engine_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_wb_counters_update_cache_updates_state(unit, engine_id);
        BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    } /* for (proc = 0; proc < unitData->num_counter_procs; proc++) */
    if (depth) {
        *depth = ctrs;
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    if (counter_info_array) {
        BCM_FREE(counter_info_array);
    }
    BCMDNX_FUNC_RETURN;
}

STATIC void
_bcm_dpp_counter_wide_mode_val_get(SOC_TMC_CNT_RESULT *data,
                                       uint64 *new_val)
{
    COMPILER_64_ZERO(*new_val);
    COMPILER_64_ADD_64(*new_val, data->byte_cnt);
    COMPILER_64_SHL(*new_val, 26);
    COMPILER_64_ADD_64(*new_val, data->pkt_cnt);
}

/*
 *   Name
 *     _bcm_dpp_counter_set_process
 *   Purpose
 *     Ensure specific counters are updated before they are accessed by
 *     updating the specific counters.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the index of the first counter to process
 *     IN unsigned int num = the number of counters to process
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is called when about to read or write a counter.  It ensures the
 *     cache is up-to-date with the current value, so that either small
 *     differences can be seen, or so that sets apply to the moment of the
 *     request rather than some unknown point in time before the request.
 */
STATIC int
_bcm_dpp_counter_set_process(_bcm_dpp_counter_state_t *unitData,
                             unsigned int proc,
                             unsigned int index,
                             unsigned int num)
{
    uint32 soc_sandResult;
    SOC_TMC_CNT_RESULT data;
    unsigned int ofs;
    int unit = unitData->unit;
    int tmpRes;
    int result = BCM_E_NONE;
#if _SATURATE_INSTEAD_OF_OVERFLOW
    uint64 oldval, new_val;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
    _bcm_dpp_counter_proc_info_t *info = &(unitData->proc[proc]);
    uint64 *counters = info->counter;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT;

    while (num > 0) {
        soc_sandResult = (MBCM_DPP_DRIVER_CALL((unit), mbcm_dpp_cnt_direct_read,
            (unit, info->proc_id, &(info->mode), index, &data)));
        tmpRes = handle_sand_result(soc_sandResult);
        if (tmpRes != BCM_E_NONE) {
            /* accounting -- read pass failure */
            info->direct_read_fails++;
            result = tmpRes;
        }
        /* accounting -- read pass successful */
        info->direct_read_passes++;
        info->cache_updates++;
        /* update this counter */
        ofs = index * _SUB_COUNT;
#if _SATURATE_INSTEAD_OF_OVERFLOW
        switch (info->mode.format) {  
        case SOC_TMC_CNT_FORMAT_PKTS:
            oldval = counters[ofs + _SUB_PKTS];
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            _bcm_dpp_counter_wide_mode_val_get(&data, &new_val);
            COMPILER_64_ADD_64(counters[ofs + _SUB_PKTS], new_val);
            if (COMPILER_64_LT(counters[ofs + _SUB_PKTS], oldval)) {
                /* don't allow wrap; saturate instead */
                COMPILER_64_ALLONES(counters[ofs + _SUB_PKTS]);
            }
            break;
        case SOC_TMC_CNT_FORMAT_BYTES:
            oldval = counters[ofs + _SUB_BYTES];
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            _bcm_dpp_counter_wide_mode_val_get(&data, &new_val);
            COMPILER_64_ADD_64(counters[ofs + _SUB_BYTES], new_val);
            if (COMPILER_64_LT(counters[ofs + _SUB_BYTES], oldval)) {
                /* don't allow wrap; saturate instead */
                COMPILER_64_ALLONES(counters[ofs + _SUB_BYTES]);
            }
            break;
        case SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE:
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            _bcm_dpp_counter_wide_mode_val_get(&data, &new_val);
            if (COMPILER_64_LT(counters[ofs + _SUB_BYTES], new_val)) {
                /* 
                 * For Max-Queue-Size, returns always the absolute maximum. 
                 * If needed, the user can zero the counter 
                 */ 
                counters[ofs + _SUB_BYTES] = new_val;
            }
            break;

        default:
            oldval = counters[ofs + _SUB_PKTS];
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            COMPILER_64_ADD_64(counters[ofs + _SUB_PKTS], data.pkt_cnt);
            if (COMPILER_64_LT(counters[ofs + _SUB_PKTS], oldval)) {
                /* don't allow wrap; saturate instead */
                COMPILER_64_ALLONES(counters[ofs + _SUB_PKTS]);
            }
            oldval = counters[ofs + _SUB_BYTES];
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            COMPILER_64_ADD_64(counters[ofs + _SUB_BYTES], data.byte_cnt);
            if (COMPILER_64_LT(counters[ofs + _SUB_BYTES], oldval)) {
                /* don't allow wrap; saturate instead */
                COMPILER_64_ALLONES(counters[ofs + _SUB_BYTES]);
            }
            break;
        }
#else /* _SATURATE_INSTEAD_OF_OVERFLOW */
       switch (info->mode.format) {
       case SOC_TMC_CNT_FORMAT_PKTS:
           _bcm_dpp_counter_wide_mode_val_get(&data, &new_val);
           COMPILER_64_ADD_64(counters[ofs + _SUB_PKTS], new_val);
           break;
       case SOC_TMC_CNT_FORMAT_BYTES:
           _bcm_dpp_counter_wide_mode_val_get(&data, &new_val);
           COMPILER_64_ADD_64(counters[ofs + _SUB_BYTES], new_val);
           break;
       case SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE:
           _bcm_dpp_counter_wide_mode_val_get(&data, &new_val);
           if (COMPILER_64_LT(counters[ofs + _SUB_BYTES], new_val)) {
               /* 
                * For Max-Queue-Size, returns always the absolute maximum. 
                * If needed, the user can zero the counter 
                */ 
               counters[ofs + _SUB_BYTES] = new_val;
           }
           break;

      default:
           COMPILER_64_ADD_64(counters[ofs + _SUB_PKTS], data.pkt_cnt);
           COMPILER_64_ADD_64(counters[ofs + _SUB_BYTES], data.byte_cnt);
           break;

       }

#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
        BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
                _bcm_dpp_wb_counters_update_counter_state(unit, proc, ofs + _SUB_PKTS),
                (COUNTER_MSG1("unit %d proc %d error updating warm boot data: counter index %d  \n"),
                         unit, proc, ofs + _SUB_PKTS),1);
        BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
                _bcm_dpp_wb_counters_update_counter_state(unit, proc, ofs + _SUB_BYTES),
                (COUNTER_MSG1("unit %d proc %d error updating warm boot data: counter index %d  \n"),
                         unit, proc, ofs + _SUB_BYTES),1);
        index++;
        num--;
    } /* while (num > 0) */
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_counters_update_direct_read_passes_state(unit, proc);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_wb_counters_update_cache_updates_state(unit, proc);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_wb_counters_update_direct_read_fails_state(unit, proc);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_bg
 *   Purpose
 *     Background handler (one thread per unit)
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *   Returns
 *     nothing
 *   Notes
 *     This sets unitData->running to TRUE to indicate it has started.
 *
 *     This thread will terminate itself and free the unit descriptor during
 *     the next iteration if unitData->running is set to FALSE.  This allows
 *     the destroy function to safely shut down the background thread by simply
 *     detaching it from the globals and setting the running flag to FALSE.
 */
void _bcm_dpp_counter_bg(void *unitPtr)
{
    _bcm_dpp_counter_state_t *unitData = unitPtr;
    unsigned int depth;
    int semTaken = FALSE;
    int mutexTaken;
    unsigned int proc_mask = 0;
    /* must be volatile so no cache attempts by compiler */
    volatile int *active = &(unitData->background_active);
    volatile int *defer = &(unitData->background_defer);
    volatile int *disable = &(unitData->background_disable);
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    if (unitData) {
        /* mark unit as running */
		unit = unitData->unit;
        unitData->running = TRUE;
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_background_collection_enable_state(unit);
        if(rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d error updating warm boot data: background_collection_enable_state  \n"),
                       unit));
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
        /* loop until told to go away */
        while (unitData->running) {
            /*
             *  This should wait for the specified interval and then give up,
             *  but we don't care whether it gets triggered or it gives up, so
             *  just discard the result deliberately.  Note that this mechanism
             *  can also be used to force an immediate trigger, such as when
             *  destroying a unit.
             */
            semTaken = (!sal_sem_take(unitData->bgSem, unitData->bgWait));
            /* indicate background process is active */
            *active = TRUE;
            
            if ((!(*defer)) && (!(*disable))) {
                /* background processing is not deferred or disabled */
                /* interlock using mutex instead of spin loops */
                mutexTaken = (0 == sal_mutex_take(unitData->cacheLock,
                                                  sal_mutex_FOREVER));
                if (!mutexTaken) {
                    /* background processing is deferred */
                    /* accounting -- background FIFO deferral */
                    unitData->fifo_read_deferred++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                    rv = _bcm_dpp_wb_counters_update_fifo_read_deferred_state(unit);
                    if(rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d error updating warm boot data: fifo_read_deferred  \n"),
                                   unit));
                    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
                    *active = FALSE;
                    continue;
                }
                /* accounting -- background FIFO read attempt */
                unitData->fifo_read_background++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_counters_update_fifo_read_background_state(unit);
                if(rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d error updating warm boot data:  fifo_read_background \n"),
                               unit));
                }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
                /* process the FIFO */
                /*process for all processors*/
                SHR_BITSET_RANGE(&proc_mask, 0, unitData->num_counter_procs);
                rv = _bcm_dpp_counter_fifo_process(unitData, proc_mask, &depth);
                if(rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d error processing counter fifo:  _bcm_dpp_counter_fifo_process \n"),
                               unit));
                }
                /*
                 *  Automatically adjust delay based upon how busy.  Tries to
                 *  reduce this thread's overhead on the system while avoiding
                 *  conditions that result in missed counter updates.
                 *
                 *  Basically, hardware docs say should read every 100ms or so
                 *  as long as we process 10 counters per FIFO read.  This
                 *  thread prefers to keep it to fewer than 8 counters per FIFO
                 *  read and so will back out to 100ms if it can, but if it
                 *  starts to get more, will reduce update delay until it hits
                 *  40ms or 8 counters per FIFO.
                 *
                 *  Note that if a foreground read occurred, it is probable that
                 *  it occurred since the last time based poll, and will
                 *  therefore skew the depth downward.  We will ignore the
                 *  adjustment to lengthen the interval in this case, to avoid
                 *  falsely making the interval longer.
                 */
                if (!(unitData->foreground_hit)) {
                    if ((_FIFO_DEPTH_MIN > depth) &&
                        (_BACKGROUND_THREAD_ITER_MAX(unit) > unitData->bgWait)) {
                        unitData->bgWait += _BACKGROUND_THREAD_ITER_ADJUST(unit);
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                        rv = _bcm_dpp_wb_counters_update_bgWait_state(unit);
                        if(rv != BCM_E_NONE) {
                            LOG_ERROR(BSL_LS_BCM_CNT,
                                      (BSL_META_U(unit,
                                                  "unit %d error updating warm boot data: bgWait  \n"),
                                       unit));
                        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
                        LOG_DEBUG(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "%s unit %d counter"
                                              " background thread delay"
                                              " to %dus\n"),
                                   "increasing",
                                   unit,
                                   unitData->bgWait));
                    }
                }
                if ((_FIFO_DEPTH_MAX(unit) < depth) &&
                    (_BACKGROUND_THREAD_ITER_MIN < unitData->bgWait)) {
                    unitData->bgWait = _BACKGROUND_THREAD_ITER_MIN;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                    rv = _bcm_dpp_wb_counters_update_bgWait_state(unit);
                    if(rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d error updating warm boot data: bgWait  \n"),
                                   unit));
                    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
                    LOG_DEBUG(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "%s unit %d counter"
                                          " background thread delay"
                                          " to %dus\n"),
                               "decreasing",
                               unit,
                               unitData->bgWait));
                } else if ((_FIFO_DEPTH_PREF(unit) < depth) &&
                    (_BACKGROUND_THREAD_ITER_MIN < unitData->bgWait)) {
                    unitData->bgWait -= _BACKGROUND_THREAD_ITER_ADJUST(unit);
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                    rv = _bcm_dpp_wb_counters_update_bgWait_state(unit);
                    if(rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d error updating warm boot data: bgWait  \n"),
                                   unit));
                    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
                    LOG_DEBUG(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "%s unit %d counter"
                                          " background thread delay"
                                          " to %dus\n"),
                               "decreasing",
                               unit,
                               unitData->bgWait));
                }
                /* no foreground hit since this FIFO run (yet) */
                unitData->foreground_hit = FALSE;
                /* allow foreground access again */
                mutexTaken = (0 == sal_mutex_give(unitData->cacheLock));
            } else { /* if (!(unitData->background_defer)) */
                /* background processing is deferred */
                /* accounting -- background FIFO deferral */
                unitData->fifo_read_deferred++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_counters_update_fifo_read_deferred_state(unit);
                if(rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d error updating warm boot data: fifo_read_deferred  \n"),
                               unit));
                }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            } /* if (!(unitData->background_defer)) */
            *active = FALSE;
        }
        /* dispose of the unit information */
        if (semTaken) {
            /*
             *  If we have it, then release it.  If not, nobody should have it
             *  now since it was released by the detach code, which is also
             *  where running was set to FALSE.
             */
            sal_sem_give(unitData->bgSem);
        }
        _bcm_counter_thread_is_running[unit] = FALSE; 
    }

    BCMDNX_FUNC_RETURN_VOID_NO_UNIT;
}

/*
 *   Name
 *     _bcm_dpp_counter_update_process
 *   Purpose
 *     Prepare for application access to a specific counter set by ensuring the
 *     specific counters are updated in the cache, and running the FIFO, to
 *     ensure that none of the specific counters were in there.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the index of the first counter to process
 *     IN unsigned int num = the number of counters to process
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This must be done before both read and set, in order to ensure that the
 *     operation applies at the time of the call rather than to some point in
 *     time an unknown interval before the call.
 *
 *     This handles the protocol between demand access and the background
 *     update thread.
 */
STATIC int
_bcm_dpp_counter_update_process(_bcm_dpp_counter_state_t *unitData,
                                unsigned int proc,
                                unsigned int index,
                                unsigned int num)
{
    int unit = unitData->unit;
    int result;
    /* must be volatile so no cache attempts by compiler */
    volatile int *defer = &(unitData->background_defer);

    BCMDNX_INIT_FUNC_DEFS;
    /* request background process deferral */
    *defer = TRUE;
    
    if (sal_mutex_take(unitData->cacheLock, sal_mutex_FOREVER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unable to take unit %d"
                                           " counter mutex"),
                          unit));
    }
    /* background process is deferred now; process the counters */
    result = _bcm_dpp_counter_set_process(unitData, proc, index, num);
    if (BCM_E_NONE == result) {
        /* successfully processed the counter set; process the FIFO */
        result = _bcm_dpp_counter_fifo_process(unitData, (1 << proc), NULL);
        unitData->foreground_hit = TRUE;
    }
    if (sal_mutex_give(unitData->cacheLock)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unable to give unit %d"
                                           " counter mutex"),
                          unit));
    }
    /* allow background process to resume */
    *defer = FALSE;
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_single_emulated
 *   Purpose
 *     Manipulate a set of sub-counters to, in effect, manipulate either a
 *     native or emulated set of counters.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the base counter of the set on which to operate
 *     IN uint32 offsets = bitmap of participating sub-counter offsets
 *     IN int update = FALSE to read, TRUE to write
 *     IN/OUT uint64 counter = where to put/get value
 *   Returns
 *     no return value
 *   Notes
 *     If emulating writes, it writes the entire value to the first sub counter
 *     it encounters, and then zeroes the rest of them.  This probably has the
 *     desired effect only in the case of zeroing an emulated counter, so it is
 *     best to only write to known 'native' counters.
 *
 *     If emulating reads, it will saturate rather than overflow when the total
 *     would not fit in 64b.
 *
 *     This is not a particularly efficient way to deal with native counters.
 */
STATIC void
_bcm_dpp_counter_single_emulated(_bcm_dpp_counter_state_t *unitData,
                                 unsigned int proc,
                                 unsigned int index,
                                 uint32 offsets,
                                 int update,
                                 uint64 *counter)
{
    uint64 accum;
#if _SATURATE_INSTEAD_OF_OVERFLOW
    uint64 prev;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
    unsigned int ofs;
    int done;
    BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT;

    if (update) {
        done = FALSE;
        for (ofs = 0; ofs < _OFS_COUNT; ofs++) {
            if (offsets & (1 << ofs)) {
                unitData->proc[proc].cache_writes++;
                if (done) {
                    COMPILER_64_ZERO(unitData->proc[proc].counter[index + ofs]);
                } else {
                    unitData->proc[proc].counter[index + ofs] = *counter;
                    done = TRUE;
                }
                BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
                    _bcm_dpp_wb_counters_update_counter_state(unit, proc, index + ofs),
                    (COUNTER_MSG1("unit %d proc %d error updating warm boot data: counter index %d  \n"),
                     unit, proc, index + ofs),1);
            } /* if (offsets & (1 << ofs)) */
        } /* for (ofs = 0; ofs <= _OFS_MAX; ofs++) */
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_cache_writes_state(unit, proc);
        if(rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d error updating warm boot data: cache_writes  \n"),
                       unit));
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    } else { /* if (update) */
        COMPILER_64_ZERO(accum);
#if _SATURATE_INSTEAD_OF_OVERFLOW
        prev = accum;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
        for (ofs = 0; ofs < _OFS_COUNT; ofs++) {
            if (offsets & (1 << ofs)) {
                unitData->proc[proc].cache_reads++;
                COMPILER_64_ADD_64(accum,
                                   unitData->proc[proc].counter[index + ofs]);
#if _SATURATE_INSTEAD_OF_OVERFLOW
                if (COMPILER_64_LT(accum, prev)) {
                    /* overflow */
                    COMPILER_64_ALLONES(accum);
                }
                prev = accum;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
            } /* if (offsets & (1 << ofs)) */
        } /* for (ofs = 0; ofs <= _OFS_MAX; ofs++) */
        *counter = accum;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_cache_reads_state(unit, proc);
        if(rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d error updating warm boot data: cache_reads  \n"),
                       unit));
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    } /* if (update) */
}



STATIC int
_bcm_dpp_counter_native(_bcm_dpp_counter_state_t *unitData,
                                 unsigned int proc,
                                 unsigned int index,
                                 uint32 offset,
                                 int update,
                                 uint64 *counter){
    int unit = unitData->unit;
	BCMDNX_INIT_FUNC_DEFS;
	BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT;

	if (update) {
		unitData->proc[proc].cache_writes++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
		rv = _bcm_dpp_wb_counters_update_cache_writes_state(unit, proc);
		BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
		unitData->proc[proc].counter[index + offset] = *counter;
		BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
		_bcm_dpp_wb_counters_update_counter_state(unit, proc, index + offset),"",0);
	}else {
		unitData->proc[proc].cache_reads++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
		rv = _bcm_dpp_wb_counters_update_cache_reads_state(unit, proc);
		BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
		*counter = unitData->proc[proc].counter[index + offset];
	} 
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
exit:
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_access
 *   Purpose
 *     Manipulate (read or write) a group of counters within a set.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN int proc = the counter processor on which to operate
 *     IN int set_id = the ID of the set on which to operate
 *     IN int update = FALSE to read, TRUE to write
 *     IN int cache_only = TRUE to only use cache values, FALSE to sync first
 *     IN _bcm_dpp_counter_format_t format = format of counter set
 *     IN unsigned int num_counters = number of counters to access
 *     IN bcm_dpp_counter_t *type = type to access (per counter)
 *     IN/OUT uint64 *counter = where to put/get value (per counter)
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This will update the cache so it is closer to being in sync with the
 *     hardware, shortly before it performs the requested operation.  The
 *     actual operation is performed against the cache.
 *
 *     When writing, a write of zero to an emulated value will write zero to
 *     all participating values.  However, a write of nonzero to an emulated
 *     value may split the value across the participants and the exact values
 *     of each participant is not guaranteed.
 *
 *     Despite the effort to keep the cache in sync with the hardware, there is
 *     still some lag.  High traffic rates will tend to make it obvious.
 *
 *     If reading a counter that is unsupported (either natively or via some
 *     emulation work), the value retrieved will be zero.  If writing such a
 *     counter, the operation is ignored quietly.
 *
 *     Because certain things are unsafe (such as yielding in an interrupt
 *     handler), this function can bypass the hardware sync and manipulate the
 *     cache only.  WARNING: in this mode, coherence of emulated counters is
 *     NOT guaranteed, nor is the point in time at which the read or write will
 *     effectively take place -- it is very possible that there is long-stale
 *     data sitting on the device that should affect the access but which will
 *     be ignored in this mode.  Set cache_only TRUE to enable this mode, but
 *     keep cache_only FALSE under normal conditions.
 */
STATIC int
_bcm_dpp_counter_access(_bcm_dpp_counter_state_t *unitData,
                        unsigned int proc,
                        unsigned int set_id,
                        int update,
                        int cache_only,
                        unsigned int num_counters,
                        const bcm_dpp_counter_t *type,
                        uint64 *counter)
{
	int unit = unitData->unit;
    int result;
    unsigned int index;
    unsigned int base;
    unsigned int num;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
    int   rv = BCM_E_NONE;
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */

    BCMDNX_INIT_FUNC_DEFS;
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d invalid counter"
                                           " processor ID %u"),
                          unit,
                          proc));
    }
    if ((!counter) || (!type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory pointer arguments must"
                                           " not be NULL")));
    }

    /*check that all counters are llegal*/
    for (index = 0; index < num_counters; index++) {
        if(unitData->proc[proc].avail_offsets[type[index]] == UNSUPPORTED_COUNTER_OFFSET){
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %u unavailable counter type %s "),
                           unit, proc, bcm_dpp_counter_t_names[type[index]]));
        }
    }

    /* how many counters to sync with hardware, according to format */
    result = _bcm_dpp_counters_per_set(unitData, proc, &num);
    BCMDNX_IF_ERR_EXIT(result);
    /* scaling is applied by hardware; we must apply it as well */
    base = set_id * num;

    if ((unitData->proc[proc].num_counters <= base) ||
        (unitData->proc[proc].num_counters <= (base + num))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d"
                                  " counter processor %u set %u,"
                                  " counters %u..%u, invalid"),
                          unit,
                          proc,
                          set_id,
                          base,
                          base + num - 1));
    }
    if (!cache_only) {
        /* sync requested counter set with hardware */
        result = _bcm_dpp_counter_update_process(unitData,
                                                 proc,
                                                 base,
                                                 num);
        BCMDNX_IF_ERR_EXIT(result);
    }
    /* now scale base so it aligns to the cache */
    base *= _SUB_COUNT;
    for (index = 0; index < num_counters; index++) {
        /*take SW cache mutex*/
        result = BCM_E_NONE;
        if (sal_mutex_take(unitData->cacheLock, sal_mutex_FOREVER)) {
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unable to take unit %d counter mutex"),
                       unit));
            result = BCM_E_CONFIG;
        }
        BCMDNX_IF_ERR_EXIT(result);

        /*if cache locked*/
        if(SHR_BITGET(unitData->proc[proc].native, type[index])){
            result = _bcm_dpp_counter_native(unitData,
                            proc,
                            base,
                            unitData->proc[proc].avail_offsets[type[index]],
                            update,
                            &(counter[index]));
            BCMDNX_IF_ERR_EXIT(result);
        }
        else{
            
            /*result = */_bcm_dpp_counter_single_emulated(unitData,
                             proc,
                             base,
                             unitData->proc[proc].avail_offsets[type[index]],
                             update,
                             &(counter[index]));
            BCMDNX_IF_ERR_EXIT(result);
        }
        /*release sw cache mutex - even if read/write failed*/ 
        if (sal_mutex_give(unitData->cacheLock)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unable to give unit %d"
                                               " counter mutex"),
                              unit));
        }
        if (BCM_E_CONFIG == result) {
            /* encountered a counter not supported in this configuration */
            COMPILER_64_ZERO(counter[index]);
            if (update) {
                unitData->proc[proc].api_miss_writes++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_counters_update_api_miss_writes_state(unit, proc);
                BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            } else {
                unitData->proc[proc].api_miss_reads++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_counters_update_api_miss_reads_state(unit, proc);
                BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            }
        } else {
            if (update) {
                unitData->proc[proc].api_writes++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_counters_update_api_writes_state(unit, proc);
                BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            } else {
                unitData->proc[proc].api_reads++;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
                rv = _bcm_dpp_wb_counters_update_api_reads_state(unit, proc);
                BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
            }
        }
    } /* for (index = 0; index < num_counters; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_clear
 *   Purpose
 *     Clear a set of counters, for allocation or similar reason.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the index of the first counter to process
 *     IN unsigned int num = the number of counters to process
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is done before returning an allocated counter set.
 */
STATIC int
_bcm_dpp_counter_clear(_bcm_dpp_counter_state_t *unitData,
                       unsigned int proc,
                       unsigned int set_id)
{
	int unit = unitData->unit;
    int result;
    unsigned int base;
    unsigned int num;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_WB_COUNTER_TRACK_DIAG_OR_CACHE_INIT;

    result = _bcm_dpp_counters_per_set(unitData, proc, &num);
    BCMDNX_IF_ERR_EXIT(result);
    base = set_id * num;
    result = _bcm_dpp_counter_update_process(unitData, proc, base, num);
    num *= _SUB_COUNT;
    base *= _SUB_COUNT;
    if (BCM_E_NONE == result) {
        while (num > 0) {
            num--;
            unitData->proc[proc].cache_writes++;
            COMPILER_64_ZERO(unitData->proc[proc].counter[base + num]);
            BCM_DPP_WB_COUNTER_SAVE_CACHE_CALL_UPDATE(
                _bcm_dpp_wb_counters_update_counter_state(unit, proc, base + num),"",0);
       }
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_cache_writes_state(unit, proc);
        if(rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d error updating warm boot data: cache_writes  \n"),
                       unit));
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    _bcm_dpp_counter_alloc
 *  Purpose
 *    Allocate a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *    IN uint32 flags = flags for allocation control
 *    IN _bcm_dpp_counter_source_t source = source for the allocated set
 *    IN/OUT int *proc = the counter processor for the counter set
 *    IN/OUT int *set_id = the ID of the counter set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if no available proc using the source
 *       BCM_E_CONFIG if we don't allocate that source here
 *       BCM_E_RESOURCE if there is no available set for the requested source
 *       BCM_E_EXSISTS if we alloc and WITH_ID and it is already in use
 *       BCM_E_* otherwise as appropriate
 *  Notes
 *    If WITH_PROC is specified instead of WITH_ID, will search for a free set
 *    on the speicied processor.
 *
 *    If WITH_ID is specified instead of WITH_PROC, will only try to allocate
 *    the specified set on the specified processor.
 *
 *    If neither WITH_ID nor WITH_PROC is specified, will search for a free set
 *    on any processor(s) that are using the requested source.
 */
STATIC int
_bcm_dpp_counter_alloc(_bcm_dpp_counter_state_t *unitData,
                       uint32 flags,
                       _bcm_dpp_counter_source_t source,
                       unsigned int *proc,
                       unsigned int *set_id)
{
	int unit = unitData->unit;
    unsigned int pIndex;
    unsigned int pLimit;
    unsigned int sIndex;
    unsigned int sLimit;
    int result = BCM_E_NOT_FOUND; /* assume no proc using the req source */
    uint8 mask;
    uint8 is_alloc;
    uint32 inUse;
    unsigned int allocated;
#ifdef BCM_WARM_BOOT_SUPPORT
    int   rv = BCM_E_NONE;
#endif /* def BCM_WARM_BOOT_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;
    if ((!proc) || (!set_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("NULL may not be user for"
                                           " obligatory OUT arguments")));
    }
    if (flags & BCM_DPP_COUNTER_WITH_ID) {
        if (*proc >= unitData->num_counter_procs) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor"
                                               " %u invalid"),
                              unit,
                              *proc));
        }
        if (*set_id >= unitData->proc[*proc].num_sets) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %u"
                                               " does not support set %u in"
                                               " current config"),
                              unit,
                              *proc,
                              *set_id));
        }
        _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, *proc);
        if (!is_alloc) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %u"
                                               " sets use implied allocation"
                                               " by source"),
                              unit,
                              *proc));
        }
        sIndex = (*set_id) >> 3;
        mask = 1 << ((*set_id) & 7);
        _bcm_dpp_wb_counters_proc_inUse_get(unit, &inUse, *proc, sIndex);
        if (inUse & mask) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %d"
                                               " set %d is already in use"),
                              unit,
                              *proc,
                              *set_id));
        }
        inUse |= mask;
        _bcm_dpp_wb_counters_proc_inUse_set(unit, inUse, *proc, sIndex);
        _bcm_dpp_wb_counters_proc_allocated_get(unit,&allocated, *proc);
        allocated++;
        _bcm_dpp_wb_counters_proc_allocated_set(unit,allocated, *proc);
        BCM_EXIT;
    } else { /* if (flags & BCM_DPP_COUNTER_WITH_ID) */
        if (flags & BCM_DPP_COUNTER_WITH_PROC) {
            pIndex = *proc;
            pLimit = *proc + 1;
        } else {
            pIndex = 0;
            pLimit = unitData->num_counter_procs;
        }
        for (; pIndex < pLimit; pIndex++) {
            if (source == unitData->proc[pIndex].source) {
                _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, pIndex);
                if (is_alloc) {
                        sLimit = (unitData->proc[pIndex].num_sets + 7) >> 3;
                        for (sIndex = 0; sIndex < sLimit; sIndex++) {
                            /* just iterate until either end or find a zero bit */
                            _bcm_dpp_wb_counters_proc_inUse_get(unit, &inUse, pIndex, sIndex);
                            if (255 != inUse){
                                break;
                            }
                        }
                        if (sIndex < sLimit) {
                            sLimit = sIndex << 3;
                            _bcm_dpp_wb_counters_proc_inUse_get(unit, &inUse, pIndex, sIndex);
                            for (mask = 1; 0 != mask; mask = mask << 1, sLimit++) {
                                /* just iterate until we find a zero bit */
                                if (0 == (mask & inUse)){
                                    break;
                                }
                            }
                            if (sLimit < unitData->proc[pIndex].num_sets) {
                                /* mark it as in use */
                                inUse |= mask;
#ifdef BCM_WARM_BOOT_SUPPORT
                                rv = _bcm_dpp_wb_counters_proc_allocated_get(unit,&allocated, pIndex);
                                BCMDNX_IF_ERR_EXIT(rv);

                                allocated++;

                                rv = _bcm_dpp_wb_counters_proc_allocated_set(unit,allocated, pIndex);
                                BCMDNX_IF_ERR_EXIT(rv);

                                rv = _bcm_dpp_wb_counters_update_allocated_state(unit, pIndex);
                                BCMDNX_IF_ERR_EXIT(rv);
#endif /*BCM_WARM_BOOT_SUPPORT*/
                                /* return it now */
                                *proc = pIndex;
                                *set_id = sLimit;
                                break;
                            } else {
                                /* fell off the end; no available sets */
                                result = BCM_E_RESOURCE;
                            }
                        } else { /* if (sIndex < sLimit) */
                            /* fell off the end; no available sets */
                            result = BCM_E_RESOURCE;
                        } /* if (sIndex < sLimit) */
                } else { /* if (is_alloc) */
                    /* we do not allocate this source here */
                    result = BCM_E_CONFIG;
                } /* if (is_alloc) */
            } /* if (source == unitData->proc[pIndex].source) */
        } /* for (; pIndex < pLimit; pIndex++) */
    } /* if (flags & BCM_DPP_COUNTER_WITH_ID) */
    
#ifdef BCM_WARM_BOOT_SUPPORT
    result = _bcm_dpp_wb_counters_update_valid_state(unit, pIndex);
    BCMDNX_IF_ERR_EXIT(result);
#endif /* BCM_WARM_BOOT_SUPPORT */

    LOG_ERROR(BSL_LS_BCM_CNT,
              (BSL_META_U(unit,
                          "unit %d unable to alloc a counter set for"
                          " source %d: %d (%s)\n"),
               unit,
               source,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    _bcm_dpp_counter_free
 *  Purpose
 *    Free a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *    IN/OUT int *proc = the counter processor for the counter set
 *    IN/OUT int *set_id = the ID of the counter set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_counter_free(_bcm_dpp_counter_state_t *unitData,
                      unsigned int proc,
                      unsigned int set_id)
{
	int unit = unitData->unit;
    unsigned int sIndex;
    uint8 mask;
    uint8 is_alloc;
    uint32 inUse;
    unsigned int allocated;

    BCMDNX_INIT_FUNC_DEFS;
    if (proc >= unitData->num_counter_procs) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor"
                                           " %u invalid"),
                          unit,
                          proc));
    }
    if (set_id >= unitData->proc[proc].num_sets) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %u"
                                           " does not support set %u in"
                                           " current config"),
                          unit,
                          proc,
                          set_id));
    }
    _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, proc);
    if (!is_alloc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %u sets"
                                           " use implied allocation by source"),
                          unit,
                          proc));
    }
    sIndex = set_id >> 3;
    mask = 1 << (set_id & 7);

    _bcm_dpp_wb_counters_proc_inUse_get(unit, &inUse, proc, sIndex);
    if (0 == (inUse & mask)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d counter processor %d set"
                                           " %d is not in use"),
                          unit,
                          proc,
                          set_id));
    }
    inUse &= (~mask);
    _bcm_dpp_wb_counters_proc_inUse_set(unit, inUse, proc, sIndex);
    _bcm_dpp_wb_counters_proc_allocated_get(unit,&allocated, proc);
    allocated--;
    _bcm_dpp_wb_counters_proc_allocated_set(unit,allocated, proc);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC
int _bcm_dpp_counter_bg_thread_attach(_bcm_dpp_counter_state_t *unit_data){
    int unit = unit_data->unit;
    BCMDNX_INIT_FUNC_DEFS;
    if(unit_data->bgSem != NULL){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d bg sem already exist\n"), unit));
    }
    if(unit_data->running){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG_NO_UNIT("unit %d bg thread already run\n"), unit));
    }
    unit_data->bgSem = sal_sem_create("dpp_counters_wait",
                                     sal_sem_BINARY,
                                     1);
    if (!unit_data->bgSem) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d unable to create bg sem\n"), unit));
    }
    unit_data->background = sal_thread_create("dpp_counter_bg",
                                                     SAL_THREAD_STKSZ,
                                                     50,
                                                     _bcm_dpp_counter_bg,
                                                     unit_data);
    if (SAL_THREAD_ERROR == unit_data->background) {
        sal_sem_destroy(unit_data->bgSem);
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d unable to create counter background thread\n"), unit));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *  Name
 *    soc_dpp_counter_bg_thread_detach
 *  Purpose
 *    Disconnect counter support on a unit
 *  Arguments
 *    int unit = unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This only signals to the background thread to stop.
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_bg_thread_detach(_bcm_dpp_counter_state_t *unit_data){
    soc_timeout_t         to;
    int unit = unit_data->unit;
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv;
#endif /* def BCM_WARM_BOOT_SUPPORT */
    BCMDNX_INIT_FUNC_DEFS;

    if (unit_data->running){
        _bcm_counter_thread_is_running[unit] = TRUE;
        unit_data->running = FALSE;
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_dpp_wb_counters_update_background_collection_enable_state(unit);
        if(rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,(_BSL_BCM_MSG_NO_UNIT("unit %d error updating warm "
                                                              "boot data: background_collection_enable_state\n"), unit));
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
        sal_sem_give(unit_data->bgSem); 
        soc_timeout_init(&to, _BCM_COUNTER_THREAD_WAIT_TIMEOUT, 0);
        while (_bcm_counter_thread_is_running[unit] == TRUE) {
            if (soc_timeout_check(&to)) {
                if (_bcm_counter_thread_is_running[unit] == TRUE) {
                    _bcm_counter_thread_is_running[unit] = FALSE;
                    /*we cannot release the bgsem because we dont sure in this case what is the thread status*/
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("unit %d _bcm_dpp_counter_bg thread is not responding\n"), unit));
                }
            }
            sal_usleep(1000000);
        }
        sal_sem_destroy(unit_data->bgSem);
        unit_data->bgSem = NULL;
        LOG_DEBUG(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit,
                              "unit %d counter background thread exit\n"), unit));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *  Name
 *    bcm_dpp_counter_dma_attach
 *  Purpose
 *    Enable the DMA from the CMIC mechanism. 
 *  Arguments
 *    int unit = unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     The only DMAs that allowed also by CRPS will be active
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_attach(int unit){
    uint32 index;
    uint32                sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    if (SOC_IS_ARAD(unit)) {
        for (index = 0; index < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); index++) {
            sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_dma_set, ((unit),index));
            BCM_SAND_IF_ERR_EXIT(sand_rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *  Name
 *    bcm_dpp_counter_dma_detach
 *  Purpose
 *    Stop the DMA from the CMIC mechanism. Still enabled by CRPS.
 *    to re-enable call bcm_dpp_counter_dma_attach
 *  Arguments
 *    int unit = unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_detach(int unit){
    uint32 index;
    uint32                sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    if (SOC_IS_ARAD(unit)) {
        for (index = 0; index < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); index++) {
            sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_dma_unset, (unit,index));
            BCM_SAND_IF_ERR_EXIT(sand_rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_dpp_counter_background_collection_enable_set(int unit, int enable){
    BCMDNX_INIT_FUNC_DEFS;

    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid")));
    }
    if(_bcm_dpp_counter_state[unit] == NULL){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("counter engine state is invalid\n")));
    }
    if(enable){
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_dma_attach(unit));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_bg_thread_attach(_bcm_dpp_counter_state[unit]));
    } else{
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_bg_thread_detach(_bcm_dpp_counter_state[unit]));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_dma_detach(unit));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_background_collection_enable_get(int unit, int *enable){
    BCMDNX_INIT_FUNC_DEFS;

    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid")));
    }
    if(_bcm_dpp_counter_state[unit] == NULL){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("counter engine state is invalid\n")));
    }
    *enable = _bcm_dpp_counter_state[unit]->running;
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_soc_dpp_str_prop_parse_counter_source(int unit, soc_port_t port, int *src_core, SOC_TMC_CNT_SRC_TYPE *src_type, int *command_id, SOC_TMC_CNT_MODE_EG_TYPE *eg_type, SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val, uint8* valid)
{
    char *propkey, propval[256], *tmp_propval;
    char             *end_c;
    int propval_len = 0, propval_index;
    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_SOURCE;
    
  
    *src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    *eg_type = SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD;

    tmp_propval = soc_property_port_get_str(unit, port, propkey);

    if (tmp_propval == NULL) {
        *valid = FALSE;
        BCM_EXIT;
    }
    *valid = TRUE;

    propval_len = sal_strlen(tmp_propval);
    for (propval_index = 0; propval_index < propval_len; propval_index++) {
        if (tmp_propval[propval_index] == '.') {
            break;
        }
    }
    if (propval_index < propval_len) {
        *src_core = _shr_ctoi(tmp_propval + propval_index);
    } else {
        *src_core = 0;
    }     
    sal_strncpy(propval,tmp_propval,propval_index);
    propval[propval_index] = '\0';

    if (isdigit((int)(propval[0]))) {
        *src_type = sal_ctoi(propval, &end_c);

        switch (*src_type)  {
        case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        case SOC_TMC_CNT_SRC_TYPE_VOQ:
        case SOC_TMC_CNT_SRC_TYPE_STAG:
        case SOC_TMC_CNT_SRC_TYPE_VSQ:
        case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_A:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_B:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_C:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_D:
            *eg_type = SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD;
             break;
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            break;
        case 6:
            *src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            break;
        case 7:
            *src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            break;
        case 8:
            *src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%d\") for %s\n\r"),*src_type, propkey));  
        }
    } else {
        if ((IS_STR_LSB0(propval, "INGRESS_FIELD")) || (IS_STR_LSB0(propval, "INGRESS_VSI"))||
           (IS_STR_LSB0(propval, "INGRESS_IN_LIF")) || (IS_STR_LSB0(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *command_id = 0;
            *we_val = 0;
        }
        else if ((IS_STR_LSB1(propval, "INGRESS_FIELD")) || (IS_STR_LSB1(propval, "INGRESS_VSI"))||
           (IS_STR_LSB1(propval, "INGRESS_IN_LIF")) || (IS_STR_LSB1(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB;
            *command_id = 1;
            *we_val = 0;
        }
        else if ((IS_STR_MSB0(propval, "INGRESS_FIELD")) || (IS_STR_MSB0(propval, "INGRESS_VSI"))||
           (IS_STR_MSB0(propval, "INGRESS_IN_LIF")) || (IS_STR_MSB0(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB;
            *command_id = 0;
            *we_val = 0;
        }
        else if ((IS_STR_MSB1(propval, "INGRESS_FIELD")) || (IS_STR_MSB1(propval, "INGRESS_VSI"))||
           (IS_STR_MSB1(propval, "INGRESS_IN_LIF")) || (IS_STR_MSB1(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_VOQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VOQ;
            *command_id = 0;
            *we_val = 0;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_VOQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VOQ;
            *command_id = 1;
            *we_val = 0;
        } 
        else if (IS_STR_LSB0(propval, "INGRESS_STAG")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_STAG;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_STAG")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_STAG;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_VSQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VSQ;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_VSQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VSQ;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_CNM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_CNM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
            *command_id = 1;
            *we_val = 0;
        }
        /*OAM*/
        else if (IS_STR_LSB0(propval, "EGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_OAM_ING_A;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_OAM_ING_B;
            *command_id = SOC_IS_JERICHO(unit) ? 0 : 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_OAM_ING_C;
            *command_id = SOC_IS_JERICHO(unit) ? 0 : 2;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "INGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_OAM_ING_D;
            *command_id = SOC_IS_JERICHO(unit) ? 0 : 3;
            *we_val = 0;
        }
        /*EPNI*/
        else if (IS_STR_LSB0(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_A;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_B;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_C;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_D;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_A;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_B;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_C;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_D;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 1;
            *we_val = 0;
        }           
        else if (IS_STR_LSB0(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_A;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_B;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_C;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_D;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_A;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_B;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_C;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI_D;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 1;
            *we_val = 0;
        }
        /*ERPP*/
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 1;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_OUTLIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_OUTLIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_FIELD")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_FIELD")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            *command_id = 0;
            *we_val = 0;
        } else  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));  
        }

        if ((IS_STR_LSB0(propval, "INGRESS_FIELD")) || (IS_STR_LSB1(propval, "INGRESS_FIELD")) || 
            (IS_STR_MSB0(propval, "INGRESS_FIELD")) || (IS_STR_MSB1(propval, "INGRESS_FIELD"))){
            *eg_type  = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
        } 
        if ((IS_STR_LSB0(propval, "INGRESS_VSI")) || (IS_STR_LSB1(propval, "INGRESS_VSI")) || 
            (IS_STR_MSB0(propval, "INGRESS_VSI")) || (IS_STR_MSB1(propval, "INGRESS_VSI"))){
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        }
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_soc_dpp_str_prop_parse_counter_format (int unit, soc_port_t port, SOC_TMC_CNT_FORMAT *p_val)
{
    char *propkey, *propval;
    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_FORMAT;
    *p_val = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;

    propval = soc_property_port_get_str(unit, port, propkey);    
    if (propval == NULL) {
        BCM_EXIT;
    }

    if ((!propval) || (sal_strcmp(propval, "PACKETS") == 0)) {
        *p_val = SOC_TMC_CNT_FORMAT_PKTS;
    } else if (sal_strcmp(propval, "BYTES") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_BYTES;
    } else if (sal_strcmp(propval, "PACKETS_AND_BYTES") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;
    } else if (sal_strcmp(propval, "MAX_QUEUE_SIZE") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE;
	}else if ((SOC_IS_ARADPLUS(unit)) && (sal_strcmp(propval, "PACKETS_AND_PACKETS") == 0)){
		*p_val = SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS;
    } else  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));

    }
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_soc_dpp_str_prop_parse_replicated_pkts(int unit, soc_port_t port, SOC_TMC_CNT_REPLICATED_PKTS *p_val)
{
    char *propkey, *propval;

    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_REPLICATED_PACKETS;

    propval = soc_property_port_get_str(unit, port, propkey);

    if ((!propval) || (sal_strcmp(propval, "ALL_COPIES") == 0)) {
        *p_val = SOC_TMC_CNT_REPLICATED_PKTS_ALL;
    } else if (sal_strcmp(propval, "FWD_COPIES") == 0) {
        *p_val = SOC_TMC_CNT_REPLICATED_PKTS_FRWRD_AND_MC;
    } else if (sal_strcmp(propval, "ONE_COPY") == 0) {
        *p_val = SOC_TMC_CNT_REPLICATED_PKTS_FRWRD;
    } else  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_soc_dpp_str_prop_parse_counter_mode(int unit, soc_port_t port, SOC_TMC_CNT_SRC_TYPE src_type, SOC_TMC_CNT_MODE_STATISTICS *stat)
{
   char *propkey, *propval;
   char             *end_c;


    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_STATISTICS;
  
    *stat = (SOC_TMC_CNT_SRC_TYPE_EGR_PP == src_type)? SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR :SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR;



    propval = soc_property_port_get_str(unit, port, propkey);    

    if (propval == NULL) {
        BCM_EXIT;
    }

    if (isdigit((int)(propval[0]))) {
        *stat = sal_ctoi(propval, &end_c);

        if (*stat > SOC_TMC_CNT_NOF_MODE_INGS_ARAD) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%d\") for %s\n\r"),*stat, propkey));  

        }   
    } else {
        if (SOC_IS_PETRAB(unit) && (sal_strcmp(propval, "FWD_COLOR") == 0))  {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI;
        } else if (SOC_IS_ARAD(unit) && (sal_strcmp(propval, "FWD_COLOR") == 0))  { /* backward support */
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI;
        } else if (SOC_IS_ARAD(unit) && (sal_strcmp(propval, "SIMPLE_COLOR") == 0))  {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI;
        } else if (sal_strcmp(propval, "FWD_DROP") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR;
        } else if (sal_strcmp(propval, "GREEN_NOT_GREEN") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW;
#ifdef BCM_88650_B0
        } else if ((sal_strcmp(propval, "FWD") == 0) && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR;
        } else if ((sal_strcmp(propval, "DROP") == 0) && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR;
#endif
        } else if (sal_strcmp(propval, "ALL") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR;
        } else if (sal_strcmp(propval, "FULL_COLOR") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI;
        }
#ifdef BCM_88660_A0
        else if ((sal_strcmp(propval, "SIMPLE_COLOR_FWD") == 0) && (SOC_IS_ARADPLUS(unit))){
            *stat = SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR;
		} else if ((sal_strcmp(propval, "SIMPLE_COLOR_DROP") == 0) && (SOC_IS_ARADPLUS(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR;
		} else if ((sal_strcmp(propval, "CONFIGURABLE") == 0) && (SOC_IS_ARADPLUS(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS;
		}
#endif
		else  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("Unexpected statistics mode value (\"%s\") for %s\n\r"),propval, propkey));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_88660_A0
STATIC int
_soc_dpp_str_prop_parse_counter_mapping(int unit, soc_port_t crps_index, uint32 *nof_counters, uint32 *entries_bmaps)
{
    uint32 propval;
    uint32 i;
    const char *offsets_propkeys[] = {spn_COUNTER_ENGINE_MAP_FWD_GREEN_OFFSET, spn_COUNTER_ENGINE_MAP_DROP_GREEN_OFFSET,
    spn_COUNTER_ENGINE_MAP_FWD_YELLOW_OFFSET, spn_COUNTER_ENGINE_MAP_DROP_YELLOW_OFFSET, spn_COUNTER_ENGINE_MAP_FWD_RED_OFFSET,
    spn_COUNTER_ENGINE_MAP_DROP_RED_OFFSET, spn_COUNTER_ENGINE_MAP_FWD_BLACK_OFFSET, spn_COUNTER_ENGINE_MAP_DROP_BLACK_OFFSET};
    const char *propkey;
    BCMDNX_INIT_FUNC_DEFS;


    /*verify map enabled*/
    propkey = spn_COUNTER_ENGINE_MAP_ENABLE;
    propval = soc_property_port_get(unit, crps_index, propkey, FALSE);
    if(!propval){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s in configurable stat mode\n\r"),propval, propkey));
    }
    /*read num of entries*/
    propkey = spn_COUNTER_ENGINE_MAP_SIZE;
    propval = soc_property_port_get(unit, crps_index, propkey, 0);    
    if((propval != 1) && (propval != 2) && (propval != 4) && (propval != 5)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s\n\r"),propval, propkey));
    }
    *nof_counters = propval;

    /*read the counters configuration*/
    for( i = 0 ; i < sizeof(offsets_propkeys)/sizeof(offsets_propkeys[0]); i++)
    {
       propval = soc_property_port_get(unit, crps_index, offsets_propkeys[i], SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL);    
       if ((propval >= *nof_counters) && (propval !=  SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL)){
           BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s\n\r"),propval, propkey));
       }
       SHR_BITSET(&(entries_bmaps[propval]), i) ;
   }

exit:
    BCMDNX_FUNC_RETURN;	
}
#endif

/****************************************************************************
 *
 *  External interface
 */


int
bcm_dpp_counter_detach(int unit)
{
    int rv;
    uint8  is_alloc;
    uint32  index;
    BCMDNX_INIT_FUNC_DEFS;

    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid")));
    }
    if (_bcm_dpp_counter_state[unit]) {
        rv = _bcm_dpp_counter_bg_thread_detach(_bcm_dpp_counter_state[unit]);
        BCMDNX_IF_ERR_CONT(rv );
    }

    rv = _bcm_dpp_counter_dma_detach(unit);
    BCMDNX_IF_ERR_CONT(rv);
    

    /* free resources*/
    if (_bcm_dpp_counter_state[unit]) {
        for (index = 0; index < _bcm_dpp_counter_state[unit]->num_counter_procs ; index++) {
            _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, index);
            if (is_alloc) {
                _bcm_dpp_wb_counters_proc_inUse_free(unit, index);
            }
        }
    }
    if (_bcm_dpp_counter_state[unit] && _bcm_dpp_counter_state[unit]->cacheLock) {
        sal_mutex_destroy(_bcm_dpp_counter_state[unit]->cacheLock);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_counters_state_deinit(unit);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit,
                              "unit %d, failed warmboot deinit, error 0x%x\n"),
                              unit,
                   rv));
        BCMDNX_IF_ERR_CONT(rv);
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
    _bcm_dpp_wb_counters_dealloc(unit);
    BCM_FREE(_bcm_dpp_counter_state[unit]);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
bcm_dpp_statistic_interface_config(int unit){
    char *propkey, *propval;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type;
    SOC_TMC_CNT_SRC_TYPE src_type;
    uint32 base_val, soc_sandRes, index;
    int src_core = 0;
    int result = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_SOURCE;
    /*config the statistic interface egress counting source. 
      we have two pointers to config*/
    for(index = 0 ; index < 2 ; index++){
        propval = soc_property_suffix_num_str_get(unit,index, propkey, "stat");

        if (propval == NULL) {
            continue;
        }
        if (sal_strcmp(propval, "EGRESS_VSI") == 0) {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        } else if (sal_strcmp(propval, "EGRESS_OUT_LIF") == 0){ 
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        } else if (sal_strcmp(propval, "EGRESS_TM") == 0){
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
        } else if (sal_strcmp(propval, "EGRESS_TM_PORT") == 0) {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
        }
        else{
            continue;
        }
        src_type = (index == 0)? SOC_TMC_CNT_SRC_TYPE_EPNI_A : SOC_TMC_CNT_SRC_TYPE_EPNI_C;
        base_val =0 ;
        
        soc_sandRes = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_epni_regs_set,(unit, src_type, src_core, eg_type, index, base_val));
        result = handle_sand_result(soc_sandRes);
        BCMDNX_IF_ERR_EXIT(result);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int 
_soc_dpp_str_prop_parse_counter_voq(
       _bcm_dpp_counter_state_t *unitData, 
       soc_port_t crps_index, 
       SOC_TMC_CNT_SRC_TYPE src_type, 
       uint32* lastVoqMin, 
       uint32* lastVoqSize, 
       SOC_TMC_CNT_VOQ_PARAMS* voq_cnt) 
{
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    unsigned int temp = 0;
    SOC_TMC_CNT_FORMAT lastVoqMinId;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(voq_cnt);
    
    SOC_TMC_CNT_VOQ_PARAMS_clear(voq_cnt);
    lastVoqMinId = unitData->proc[crps_index].mode.format;
    temp = soc_property_port_get(unit, crps_index, spn_COUNTER_ENGINE_VOQ_MIN_QUEUE, lastVoqMin[lastVoqMinId]);

    if ((lastVoqMinId != SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE) && 
        ((temp < lastVoqMin[lastVoqMinId]) || (temp < lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES]))) {
        LOG_ERROR(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit, "unit %d counter processor %u VOQ start queue %u overlaps prior counter processor VOQ space\n"),
                                    unit, crps_index, temp));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    } else if (SOC_DPP_DEFS_GET(unit, nof_queues) > temp) {
        lastVoqMin[lastVoqMinId] = temp;
        voq_cnt->start_q = temp;
        temp = soc_property_port_get(unit, crps_index, spn_COUNTER_ENGINE_VOQ_QUEUE_SET_SIZE, *lastVoqSize);

        switch (temp) {
            case 1:
                voq_cnt->q_set_size = SOC_TMC_CNT_Q_SET_SIZE_1_Q;
                break;
            case 2:
                voq_cnt->q_set_size = SOC_TMC_CNT_Q_SET_SIZE_2_Q;
                break;
            case 3:
                LOG_WARN(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d counter processor %u VOQ queues per group %u is not valid; adjusting to %u\n"),
                                                    unit, crps_index, temp, 4));
                temp = 4;
                /* fallthrough intentional */
            case 4:
                voq_cnt->q_set_size  = SOC_TMC_CNT_Q_SET_SIZE_4_Q;
                break;
            case 5:
            case 6:
            case 7:
                LOG_WARN(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d counter processor %d VOQ queues per group %d is not valid; adjusting to %d\n"),
                                            unit, crps_index, temp, 8));
                temp = 8;
                /* fallthrough intentional */
            case 8:
                voq_cnt->q_set_size = SOC_TMC_CNT_Q_SET_SIZE_8_Q;
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d counter processor %u VOQ queues per group %u is not valid (max 8)\n"),
                                           unit, crps_index, temp));
                BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        BCMDNX_IF_ERR_EXIT(result);
        
        *lastVoqSize = temp;
        LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d proc %d VOQ start %u, queues per set %d\n"),
                                            unit, crps_index, voq_cnt->start_q, temp));       
    } else {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processor %u VOQ start queue %u invalid\n"),
                                            unit, crps_index, temp));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    /* compute where this processor's queus space ends if VOQ mode */
    lastVoqMin[lastVoqMinId] += (unitData->proc[crps_index].num_sets * (*lastVoqSize));
    lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES] = 
        SOC_SAND_MAX(lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES],
                     SOC_SAND_MIN(lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS], lastVoqMin[SOC_TMC_CNT_FORMAT_BYTES]));
    if (lastVoqMin[lastVoqMinId] > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
        LOG_WARN(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processor %u VOQ max %u exceeds supported hardware range; counters will be wasted.\n"),
                                  unit, crps_index, lastVoqMin[lastVoqMinId]));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_soc_dpp_str_prop_parse_counter_stag(
   _bcm_dpp_counter_state_t *unitData, 
   soc_port_t crps_index, 
   SOC_TMC_CNT_SRC_TYPE src_type, 
   uint32* stag_lsb)
{
    int unit = unitData->unit;
    unsigned int temp = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(stag_lsb);
    
    if (unitData->global_info.haveStatTag) { 
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d already has at least one processor in stat tag mode\n"),unit));
        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
    }
    LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d proc %u source %s\n"), unit, crps_index, "SOC_TMC_CNT_SRC_TYPE_STAG"));
    temp = soc_property_port_get(unit, crps_index, spn_COUNTER_ENGINE_STAG_LOW_BIT, 0);
    if (temp >= 16) {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processor %u statistics tag lsb %u not valid\n"), unit, crps_index, temp));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    *stag_lsb = temp;
    LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d proc %d stag lsb %u\n"), unit, crps_index, temp));        
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_source_config_get(
       _bcm_dpp_counter_state_t *unitData,
       soc_port_t crps_index, 
       int *src_core,
       SOC_TMC_CNT_SRC_TYPE* src_type, 
       int *command_id,
       SOC_TMC_CNT_MODE_EG_TYPE* eg_type,
       SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val,
       SOC_TMC_CNT_VOQ_PARAMS* voq_cnt) {
    int unit = unitData->unit;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(src_type);
    BCMDNX_NULL_CHECK(command_id);
    BCMDNX_NULL_CHECK(eg_type);
    BCMDNX_NULL_CHECK(voq_cnt);

    *src_type = unitData->proc[crps_index].mode.src_type;
    *eg_type =  unitData->proc[crps_index].mode.mode_eg.type;
    *src_core = unitData->proc[crps_index].mode.src_core;
    *command_id = unitData->proc[crps_index].mode.command_id;

    if (unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_VOQ) {
        voq_cnt->q_set_size = unitData->proc[crps_index].mode.voq_cnt.q_set_size;
        voq_cnt->start_q = unitData->proc[crps_index].mode.voq_cnt.start_q;
    }
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int 
_bcm_dpp_counter_source_config(
       _bcm_dpp_counter_state_t *unitData,
       soc_port_t crps_index, 
       int src_core,
       SOC_TMC_CNT_SRC_TYPE src_type, 
       int command_id,
       SOC_TMC_CNT_MODE_EG_TYPE eg_type,
       SOC_TMC_CNT_COUNTER_WE_BITMAP we_val,
       SOC_TMC_CNT_VOQ_PARAMS* voq_cnt,
       uint32 stag_lsb) {
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    int index = crps_index;
    SOC_TMC_CNT_Q_SET_SIZE q_set_size;
    uint32 start_q;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(voq_cnt);

    q_set_size = voq_cnt->q_set_size;
    start_q = voq_cnt->start_q;
    unitData->proc[crps_index].mode.src_core = src_core;
    unitData->proc[crps_index].mode.src_type = src_type;
    unitData->proc[crps_index].source = src_type;
    unitData->proc[crps_index].mode.command_id = command_id;
    unitData->proc[crps_index].mode.we_val = we_val;
    switch (src_type) {
        case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        case SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB:
        case SOC_TMC_CNT_SRC_TYPE_VSQ:
        case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
        case SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB:
        case SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_A:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_B:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_C:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_D:
            unitData->proc[index].mode.mode_eg.type = eg_type;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u source %s\n"),
                       unit,
                       index,
                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[index].mode.src_type)));
            break;
        case SOC_TMC_CNT_SRC_TYPE_VOQ:
            SOC_TMC_CNT_VOQ_PARAMS_clear(&(unitData->proc[index].mode.voq_cnt));
            unitData->proc[index].mode.voq_cnt.q_set_size = q_set_size;
            unitData->proc[index].mode.voq_cnt.start_q = start_q;
            LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d proc %u source %s\n"), unit, index, "SOC_TMC_CNT_SRC_TYPE_VOQ"));
            break;
        case SOC_TMC_CNT_SRC_TYPE_STAG:
            if (unitData->global_info.haveStatTag) { 
                LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d already has at least one processor in stat tag mode\n"),unit));
                BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
            }  
            unitData->proc[index].mode.stag_lsb = stag_lsb;
            unitData->global_info.haveStatTag = TRUE;           
            break;
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:         
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB:
            SOC_TMC_CNT_MODE_EG_clear(&(unitData->proc[index].mode.mode_eg));
            unitData->proc[index].mode.mode_eg.type = eg_type;

            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u source %s egress type %s\n"),
                       unit,
                       index,
                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[index].mode.src_type),
                       SOC_TMC_CNT_MODE_EG_TYPE_to_string(unitData->proc[index].mode.mode_eg.type)));
            break;
#ifdef BCM_ARAD_SUPPORT
        case SOC_TMC_CNT_SRC_TYPE_EPNI_A:
        case SOC_TMC_CNT_SRC_TYPE_EPNI_B:
        case SOC_TMC_CNT_SRC_TYPE_EPNI_C:
        case SOC_TMC_CNT_SRC_TYPE_EPNI_D:
#endif          
            unitData->proc[index].mode.mode_eg.type = eg_type;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u source %s egress type %s\n"),
                       unit,
                       index,
                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[index].mode.src_type),
                       SOC_TMC_CNT_MODE_EG_TYPE_to_string(unitData->proc[index].mode.mode_eg.type)));
            break;    
    
        default:
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d counter processor %u"
                                  " invalid source %u\n"),
                       unit,
                       index,
                       src_type));
            result = BCM_E_PARAM;
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_proc_inUse_config(_bcm_dpp_counter_state_t *unitData, soc_port_t crps_index){
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    size_t allocSize = 0; 
    uint8 is_alloc = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    
    result = _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, crps_index);
    BCMDNX_IF_ERR_EXIT(result);
    if (is_alloc) {
        BCM_EXIT;    
    }

    switch (unitData->proc[crps_index].source) {
        case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        case SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB:
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB:
        case SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB:
        case SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB:
            /* we manage these locally */
            allocSize = ((unitData->proc[crps_index].num_sets + 7) / 8) * sizeof(uint8);
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "allocate %u bytes for unit %d"
                                  " proc %u set allocation\n"),
                       (uint32)allocSize,
                       unit,
                       crps_index));
            result = _bcm_dpp_wb_counters_proc_inUse_allocate(unit, allocSize, crps_index);
            BCMDNX_IF_ERR_EXIT(result);
            break;
        default:
            /* these are implied or managed elsewhere */
            break;
        }
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_dpp_counter_mode_config(_bcm_dpp_counter_state_t *unitData, soc_port_t crps_index, SOC_TMC_CNT_MODE_STATISTICS stat) {
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    unsigned int index2;
    
    uint32 *entries_bmaps = unitData->proc[crps_index].mode.custom_mode_params.entries_bmaps;
    uint32 offset = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    
    switch(unitData->proc[crps_index].mode.src_type){
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_A:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_B:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_C:
        case SOC_TMC_CNT_SRC_TYPE_OAM_ING_D:
            if (stat != SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR && stat != SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS){
                   LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processor %u invalid stat %s for OAM\n"),
                                             unit,crps_index, SOC_TMC_CNT_MODE_STATISTICS_to_string(stat)));
                   BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
            if (stat != SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR && stat != SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS){
                LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processor %u invalid stat %s for CNM\n"),
                           unit,crps_index,SOC_TMC_CNT_MODE_STATISTICS_to_string(stat)));
                BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case SOC_TMC_CNT_SRC_TYPE_EPNI_A:
        case SOC_TMC_CNT_SRC_TYPE_EPNI_B:
        case SOC_TMC_CNT_SRC_TYPE_EPNI_C:
        case SOC_TMC_CNT_SRC_TYPE_EPNI_D:
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB:
            if(unitData->proc[crps_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM || 
               unitData->proc[crps_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT){
                if((stat != SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR) && (stat != SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR) &&
                    (stat != SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR) && (stat != SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS)){
                    LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processor %u invalid stat %s for %s\n"),
                                unit,crps_index, SOC_TMC_CNT_MODE_STATISTICS_to_string(stat), SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type)));
                    BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
                }
            }
            break;
        default:
            break;
    }

            
    if (SOC_IS_JERICHO(unit)) {
        unitData->proc[crps_index].mode.mode_statistics = SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS;
    } else {
        unitData->proc[crps_index].mode.mode_statistics = stat;
    }
    /* configure things according to the specified stats format */
    switch (stat) {
        case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI: /*SIMPLE_COLOR*/
            if (SOC_IS_PETRAB(unit)) {
                unitData->proc[crps_index].mode.mode_eg.resolution = SOC_TMC_CNT_MODE_EG_RES_COLOR;
                unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 2;
                /* hardware supported counters */
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_fwd_green_pkts);
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_fwd_green_bytes);
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_fwd_yellow_pkts);
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_fwd_yellow_bytes);
                /* all supported counters (native and renamed/emulated) */
                offset =  ((1 << (_OFS_E_CY_GREEN + _SUB_PKTS)) | (1 << (_OFS_E_CY_YELLOW + _SUB_PKTS)));
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_not_red_pkts] = offset; 
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_not_red_pkts] = offset;
                
                offset = ((1 << (_OFS_E_CY_GREEN + _SUB_BYTES)) | (1 << (_OFS_E_CY_YELLOW + _SUB_BYTES)));
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_bytes] = offset; 
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_not_red_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_not_red_bytes] = offset;

                offset = _OFS_E_CY_GREEN + _SUB_PKTS;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_green_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_not_yellow_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_green_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_not_yellow_pkts] = offset;

                offset = _OFS_E_CY_GREEN + _SUB_BYTES;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_green_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_not_yellow_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_green_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_not_yellow_bytes] = offset;

                offset = _OFS_E_CY_YELLOW + _SUB_PKTS;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_yellow_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_not_green_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_yellow_pkts] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_not_green_pkts] = offset;
                
                offset = _OFS_E_CY_YELLOW + _SUB_BYTES;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_yellow_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_not_green_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_yellow_bytes] = offset;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_not_green_bytes] = offset;
                LOG_DEBUG(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d proc %u mode %s,"
                                      " sets %u\n"),
                           unit,
                           crps_index,
                           "SOC_TMC_CNT_MODE_EG_RES_COLOR",
                           unitData->proc[crps_index].num_sets));
            } else {
                unitData->proc[crps_index].mode.mode_eg.resolution = SOC_TMC_CNT_MODE_EG_RES_COLOR;
                unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 2;
                unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 2;
                entries_bmaps[0] = GREEN_FWD | GREEN_DROP;
                entries_bmaps[1] = YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP | BLACK_FWD | BLACK_DROP;
                LOG_DEBUG(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d proc %u mode %s,"
                                      " sets %u\n"),
                           unit,
                           crps_index,
                           "SOC_TMC_CNT_MODE_EG_RES_COLOR",
                           unitData->proc[crps_index].num_sets));
            }
            break;
#ifdef BCM_88660_A0
        case SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS: /*CONFIGURABLE: */
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / unitData->proc[crps_index].mode.custom_mode_params.nof_counters;
            LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d proc %u mode %s, sets %u\n"),
                       unit,crps_index,"SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS",unitData->proc[crps_index].num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR: /*SIMPLE_COLOR_FWD:*/
            unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 2;
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 2;
            entries_bmaps[0] = GREEN_FWD;
            entries_bmaps[1] = YELLOW_FWD | RED_FWD | BLACK_FWD;
            entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR",
                       unitData->proc[crps_index].num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR: /*SIMPLE_COLOR_DROP:*/
            unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 2;
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 2;
            entries_bmaps[0] = GREEN_DROP;
            entries_bmaps[1] = YELLOW_DROP | RED_DROP | BLACK_DROP;
            entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR",
                       unitData->proc[crps_index].num_sets));
            break;
#endif
        case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR: /*FWD_DROP*/
            unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 2;
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 2;
            entries_bmaps[0] = GREEN_FWD | YELLOW_FWD| RED_FWD | BLACK_FWD;
            entries_bmaps[1] =  GREEN_DROP | YELLOW_DROP  | RED_DROP | BLACK_DROP;
#ifdef BCM_88660_A0
            if((SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) && (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)){
                unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters;
                /*packet and packet is only fwd_drop. and we use implict decleartion of native and avail formats*/
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_fwd_pkts);
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_drop_pkts);
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_pkts] = ((1 << (_OFS_E_CN + _SUB_FWD_PKTS)) |
                                                                                  (1 << (_OFS_E_CN + _SUB_DROP_PKTS)));
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_pkts] = _OFS_E_CN + _SUB_FWD_PKTS;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_drop_pkts] =  _OFS_E_CN + _SUB_DROP_PKTS;
            }
#endif
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR",
                       unitData->proc[crps_index].num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW: /*GREEN_NOT_GREEN:*/
            unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 4;
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 4;
            entries_bmaps[0] = GREEN_FWD;
            entries_bmaps[1] = GREEN_DROP;
            entries_bmaps[2] = YELLOW_FWD | RED_FWD | BLACK_FWD;
            entries_bmaps[3] = YELLOW_DROP | RED_DROP | BLACK_DROP;   
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW",
                       unitData->proc[crps_index].num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI: /*FULL_COLOR*/
            unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 5;
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters / 5;
            entries_bmaps[0] = GREEN_FWD;
            entries_bmaps[1] = GREEN_DROP;
            entries_bmaps[2] = YELLOW_FWD | RED_FWD | BLACK_FWD;
            entries_bmaps[3] = YELLOW_DROP | RED_DROP;  
            entries_bmaps[4] = BLACK_DROP; 
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI",
                       unitData->proc[crps_index].num_sets));
            break;
#ifdef BCM_88650_B0
        case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR: /*FWD:*/
            if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
                unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 1;
                unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters;
                entries_bmaps[0] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
                entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
                LOG_DEBUG(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d proc %u mode %s,"
                                      " sets %u\n"),
                           unit,
                           crps_index,
                           "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR_ONE_ENTRY",
                           unitData->proc[crps_index].num_sets));
            }
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR: /*DROP:*/ 
            if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
                unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 1;
                unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters;
                entries_bmaps[0] = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
                entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
                LOG_DEBUG(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d proc %u mode %s,"
                                      " sets %u\n"),
                           unit,
                           crps_index,
                           "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR_ONE_ENTRY",
                           unitData->proc[crps_index].num_sets));
            }
            break;
#endif
        case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR: /*ALL:*/
            unitData->proc[crps_index].mode.custom_mode_params.nof_counters = 1;
            unitData->proc[crps_index].num_sets = unitData->proc[crps_index].num_counters;
            entries_bmaps[0] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD | GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR_ONE_ENTRY",
                       unitData->proc[crps_index].num_sets));
            break;

        default:
            /* this was already handled above */
            break;
    }
    BCMDNX_IF_ERR_EXIT(result);

    /*except the PetraB and the packets and packets mode we calc the avail and native format by the entries bmap */
    if (!SOC_IS_PETRAB(unit) && unitData->proc[crps_index].mode.format != SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) {
        result = _bcm_dpp_counters_calc_formats(
                   unitData->proc[crps_index].num_counters/unitData->proc[crps_index].num_sets, 
                   entries_bmaps, 
                   unitData->proc[crps_index].native, 
                   unitData->proc[crps_index].avail_offsets);
        BCMDNX_IF_ERR_EXIT(result);
    }

    /* Remove from native and available in case of packet-only or byte-only the other counters */
    /*remove all the packets formats*/
    if (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_BYTES) {
        uint8 is_bytes_format;
        for (index2 = 0; index2 < bcm_dpp_counter_count ; index2++) {
            result = _bcm_dpp_counters_is_byte_format_get(index2, &is_bytes_format);
            BCMDNX_IF_ERR_EXIT(result);

            if(!is_bytes_format){
                SHR_BITCLR(unitData->proc[crps_index].native, index2);
                unitData->proc[crps_index].avail_offsets[index2] = UNSUPPORTED_COUNTER_OFFSET;
            }
        }
    } else if ((unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS) || 
               (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)) { /*remove bytes formats*/
        uint8 is_bytes_format;
        for (index2 = 0; index2 < bcm_dpp_counter_count; index2++) {
            result = _bcm_dpp_counters_is_byte_format_get(index2, &is_bytes_format);
            BCMDNX_IF_ERR_EXIT(result);

            if(is_bytes_format){
                SHR_BITCLR(unitData->proc[crps_index].native, index2);
                unitData->proc[crps_index].avail_offsets[index2] = UNSUPPORTED_COUNTER_OFFSET;
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_unit_state_verify(_bcm_dpp_counter_state_t *unitData, soc_port_t crps_index) {
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    SOC_TMC_CNT_SRC_TYPE src_ptr_a;
    SOC_TMC_CNT_SRC_TYPE src_ptr_b;
    unsigned int j;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    
    if (!unitData->proc[crps_index].valid) {
        BCM_EXIT;
    }
    if (unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_VOQ){
        BCM_EXIT;
    }
    result = _bcm_dpp_counter_get_counter_src_pointer(unitData->proc[crps_index].mode.src_type, unitData->proc[crps_index].mode.src_core, &src_ptr_a);
    BCMDNX_IF_ERR_EXIT(result);

    for (j = 0; (j < unitData->num_counter_procs); j++) {
        if (j == crps_index) {
            continue;
        }
        if (unitData->proc[j].mode.src_type == SOC_TMC_CNT_SRC_TYPE_VOQ){
            continue;
        }
        if (!unitData->proc[j].valid) {
            continue;
        }
        result = _bcm_dpp_counter_get_counter_src_pointer(unitData->proc[j].mode.src_type, unitData->proc[j].mode.src_core, &src_ptr_b);
        BCMDNX_IF_ERR_EXIT(result);

        if (src_ptr_a == src_ptr_b){
            if(unitData->proc[crps_index].mode.mode_statistics != unitData->proc[j].mode.mode_statistics) {
                LOG_ERROR(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d different ingress mode defined for source %s proc %d: %s , proc %d: %s\n"),
                           unit,
                           SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                           crps_index,
                           SOC_TMC_CNT_MODE_STATISTICS_to_string(unitData->proc[crps_index].mode.mode_statistics),
                           j,
                           SOC_TMC_CNT_MODE_STATISTICS_to_string(unitData->proc[j].mode.mode_statistics)));
                BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
            } else { /*unitData->proc[crps_index].mode.mode_statistics == unitData->proc[j].mode.mode_statistics*/
                if((unitData->proc[crps_index].mode.format != unitData->proc[j].mode.format) && 
                   (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS || unitData->proc[j].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)) {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d can't use packet and packet and another format for the same source type %s proc %d: %s , proc %d: %s\n"),
                               unit,
                               SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                               crps_index,
                               SOC_TMC_CNT_FORMAT_to_string(unitData->proc[crps_index].mode.format),
                               j,
                               SOC_TMC_CNT_FORMAT_to_string(unitData->proc[j].mode.format)));
                    BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
                }
                if(unitData->proc[crps_index].mode.mode_statistics == SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS &&
                   (sal_memcmp(&(unitData->proc[crps_index].mode.custom_mode_params), &(unitData->proc[j].mode.custom_mode_params) , sizeof(unitData->proc[crps_index].mode.custom_mode_params))!= 0)){
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d different configurable mode params for source %s proc %d and proc %d\n"),
                               unit,
                               SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                               crps_index,
                               j));
                    BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
                }
                if (src_ptr_a == SOC_TMC_CNT_SRC_TYPE_EGR_PP) { 
                    /*check if we wish to configure Egress VSI/PMF coungting*/
                    if (unitData->proc[crps_index].mode.src_type == unitData->proc[j].mode.src_type) { 
                        /*check that we wish to configure the same command ID*/
                        if (unitData->proc[crps_index].mode.mode_eg.type != unitData->proc[j].mode.mode_eg.type) { 
                            /*the mode_eg must be the same, i.e. we must count the same entity on the same command id*/
                            LOG_ERROR(BSL_LS_BCM_CNT,
                                      (BSL_META_U(unit,
                                                  "unit %d different counting mdoes for the same command_id params for source %s proc %d and proc %d\n"),
                                       unit,
                                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                       crps_index,
                                       j));
                            BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);

                        }
                    }
                }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_oam_verify(_bcm_dpp_counter_state_t *unitData) {
    unsigned int index;
    int first_oam_ing_proc = -1;
    int first_oam_eng_proc = -1;
    int unit = unitData->unit;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    for (index = 0; (index < unitData->num_counter_procs); index++) {
        switch(unitData->proc[index].mode.src_type){
            case SOC_TMC_CNT_SRC_TYPE_OAM_ING_A:
            case SOC_TMC_CNT_SRC_TYPE_OAM_ING_B:
                if(first_oam_eng_proc == -1){
                    first_oam_eng_proc = index;
                }
               else if ((index >= 2) && (first_oam_eng_proc < 2)){
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d Both OAM ENG counter engines must be {0,1} or {2,3}. Your configuration {%d,%d}"),
                               unit, first_oam_ing_proc , index));
                    result = BCM_E_CONFIG;
                    break; 
                }
                break;
            case SOC_TMC_CNT_SRC_TYPE_OAM_ING_C:           
            case SOC_TMC_CNT_SRC_TYPE_OAM_ING_D:
                if(first_oam_ing_proc == -1){
                    first_oam_ing_proc = index;
                }
                else if ((index >= 2) && (first_oam_ing_proc < 2)){
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d Both OAM ING counter engines must be {0,1} or {2,3}. Your configuration {%d,%d}"),
                               unit, first_oam_ing_proc, index));
                    result = BCM_E_CONFIG;
                    break;
                }
                break;
            default:
                continue;
        }
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_counter_config_set(
        int unit,
        soc_port_t crps_index,
        int src_core,
        SOC_TMC_CNT_SRC_TYPE src_type,
        int command_id,
        SOC_TMC_CNT_MODE_EG_TYPE eg_type,
        SOC_TMC_CNT_FORMAT format,
        SOC_TMC_CNT_COUNTER_WE_BITMAP we_val,
        SOC_TMC_CNT_CUSTOM_MODE_PARAMS* custom_mode_params,
        SOC_TMC_CNT_VOQ_PARAMS* voq_cnt,
        uint32 stag_lsb) {
    _bcm_dpp_counter_state_t *unitData;
    int result = BCM_E_NONE;
    uint32 soc_sandRes;
    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_NULL_CHECK(custom_mode_params);

    SOC_TMC_CNT_COUNTERS_INFO_clear(&(unitData->proc[crps_index].mode));
    SOC_TMC_CNT_MODE_EG_clear(&(unitData->proc[crps_index].mode.mode_eg));
    /* get counter processor source */
    /* all but last default to ingress PP, last to egress PP */
    unitData->proc[crps_index].mode.format = format;
    result = _bcm_dpp_counter_source_config(unitData, crps_index, src_core, src_type, command_id, eg_type, we_val, voq_cnt, stag_lsb);
    BCMDNX_IF_ERR_EXIT(result);
    
    sal_memcpy(unitData->proc[crps_index].mode.custom_mode_params.entries_bmaps, custom_mode_params->entries_bmaps, sizeof(uint32)*SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    unitData->proc[crps_index].mode.custom_mode_params.nof_counters = custom_mode_params->nof_counters;
    

    result = _bcm_dpp_counter_mode_config(unitData, crps_index, SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_proc_inUse_config(unitData, crps_index);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_unit_state_verify(unitData, crps_index);
    BCMDNX_IF_ERR_EXIT(result);

    /* check oam configuration is valid*/
    result = _bcm_dpp_counter_oam_verify(unitData);
    BCMDNX_IF_ERR_EXIT(result);

    /*Commit to HW*/
    soc_sandRes = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_counters_set,(unit, unitData->proc[crps_index].proc_id, &(unitData->proc[crps_index].mode))));
    result = handle_sand_result(soc_sandRes);
    BCMDNX_IF_ERR_EXIT(result);

    unitData->proc[crps_index].valid = TRUE;

#ifdef BCM_WARM_BOOT_SUPPORT
    result = _bcm_dpp_wb_counters_update_mode_state(unit, crps_index);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_wb_counters_update_valid_state(unit, crps_index);
    BCMDNX_IF_ERR_EXIT(result);
#endif /*BCM_WARM_BOOT_SUPPORT*/

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_counter_config_get(
        int unit,
        soc_port_t crps_index,
        uint8* enabled,
        int* src_core,
        SOC_TMC_CNT_SRC_TYPE* src_type,
        int* command_id,
        SOC_TMC_CNT_MODE_EG_TYPE* eg_type,
        SOC_TMC_CNT_FORMAT* format,
        SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val,
        SOC_TMC_CNT_CUSTOM_MODE_PARAMS* custom_mode_params,
        SOC_TMC_CNT_VOQ_PARAMS* voq_cnt,
        uint32* stag_lsb){
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    /*uint8 is_alloc;*/
    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_NULL_CHECK(enabled);
    BCMDNX_NULL_CHECK(src_core);
    BCMDNX_NULL_CHECK(src_type);
    BCMDNX_NULL_CHECK(eg_type);
    BCMDNX_NULL_CHECK(we_val);
    BCMDNX_NULL_CHECK(voq_cnt);
    BCMDNX_NULL_CHECK(custom_mode_params);
    
    *enabled = unitData->proc[crps_index].valid;
    if (!unitData->proc[crps_index].valid) {
        BCM_EXIT;
    }
    result = _bcm_dpp_counter_source_config_get(unitData, crps_index, src_core, src_type, command_id, eg_type, we_val, voq_cnt);
    BCMDNX_IF_ERR_EXIT(result);

    custom_mode_params->nof_counters = unitData->proc[crps_index].mode.custom_mode_params.nof_counters;
    sal_memcpy(custom_mode_params->entries_bmaps, 
               unitData->proc[crps_index].mode.custom_mode_params.entries_bmaps, 
               sizeof(uint32) * SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    *format = unitData->proc[crps_index].mode.format;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_config_clear(int unit, soc_port_t crps_index){
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_init(int unit)
{
    _bcm_dpp_counter_state_t *tempUnit = NULL;

    int result = BCM_E_NONE;
    uint32 soc_sandRes;
    unsigned int index;
    unsigned int counters;
    uint32 entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_COUNT]; 
    /* 0-Regular 1 - Packet-only, 2 - Byte only, 3-Queue-size 4-unused 5- Packet-and-Packet*/
    unsigned int lastVoqMin[SOC_TMC_CNT_NOF_FORMATS_JERICHO] = {0, 0, 0, 0, 0, 0};
    unsigned int lastVoqSize = 1;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type;
    SOC_TMC_CNT_SRC_TYPE src_type;
    SOC_TMC_CNT_VOQ_PARAMS voq_cnt;
    SOC_TMC_CNT_MODE_STATISTICS stat;
    SOC_TMC_CNT_COUNTER_WE_BITMAP we_val = 0;
    int src_core = 0;
    int command_id = 0;
    uint8 valid = FALSE;
    uint32 stag_lsb = 0;
#ifdef BCM_WARM_BOOT_SUPPORT
    int wbInit = FALSE;
#endif /* def BCM_WARM_BOOT_SUPPORT */
    uint64 *baseCounter;
    uint8 is_alloc;
    size_t nof_procs;
    size_t allocSize = 0; 

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_counters_def_init();

    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid")));
    }
    if (_bcm_dpp_counter_state[unit]) {
        /* unit is already inited, detach first then reinit */
        result = bcm_dpp_counter_detach(unit);
    }
    BCMDNX_IF_ERR_EXIT(result);
   
    /* compute total size of counter state for this unit */               
    counters = (SOC_DPP_DEFS_GET(unit, nof_counter_processors) * SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)) + 
        (SOC_DPP_DEFS_GET(unit, nof_small_counter_processors) * SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor));/* processor descriptor * counter processors */
    nof_procs = (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)); /* processor descriptor */
    allocSize = (sizeof(*tempUnit)) + /* unit descriptor */
                (sizeof(*(tempUnit->proc)) * nof_procs) + /* counter cache */ 
                (sizeof(*(tempUnit->proc->counter)) * counters * _SUB_COUNT) + /*SW data structure for counter statistics*/
                (sizeof(*(tempUnit->fifo_results.cnt_result)) * SOC_DPP_DEFS_GET(unit, counter_fifo_depth)); /*Array of the cache counters. Only the first 'counter_fifo_depth' counters are relevant.*/
    LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"allocate %u bytes for unit %d counter information and cache: %u processors, %u total counters, %u sub per counter\n"),
                                            (uint32)allocSize, unit,SOC_DPP_DEFS_GET(unit, nof_counter_processors), counters, _SUB_COUNT));
    /* try to allocate a sufficiently large memory cell */
    result = _bcm_dpp_wb_counters_counter_state_allocate(unit, nof_procs);
    BCMDNX_IF_ERR_EXIT(result);

    BCMDNX_ALLOC(tempUnit, allocSize, "DPP counter state");
    if (!tempUnit) {
        LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unable to allocate %u bytes for unit %d counter information and cache (%u processors, %u total counters (%u sub per counter)\n"),
                                                    (uint32)allocSize, unit, SOC_DPP_DEFS_GET(unit, nof_counter_processors), counters, _SUB_COUNT));
        result = BCM_E_MEMORY;
        BCMDNX_IF_ERR_EXIT(result);
    }

    /* got the memory; set up initial state */
    sal_memset(tempUnit, 0x00, allocSize);
    tempUnit->unit = unit;
    tempUnit->num_counter_procs = nof_procs;
    /* point to processor descriptor array */
    tempUnit->proc = (_bcm_dpp_counter_proc_info_t*)(&(tempUnit[1]));
    
    /* figure out where counter cache starts */
    baseCounter = (uint64*)(&(tempUnit->proc[tempUnit->num_counter_procs]));
    /* set up each processor */
    counters = 0;
    for (index = 0; index < tempUnit->num_counter_procs; index++) {
        uint32 i;
        
        tempUnit->proc[index].proc_id = SOC_TMC_CNT_PROCESSOR_ID_A + index;
        tempUnit->proc[index].num_counters = 
            (index < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) ? 
            SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) : 
            SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor);
        tempUnit->proc[index].counter = &(baseCounter[counters]);
        counters += (tempUnit->proc[index].num_counters * _SUB_COUNT);
        for (i = 0 ; i < bcm_dpp_counter_count ; i++){
            tempUnit->proc[index].avail_offsets[i] = UNSUPPORTED_COUNTER_OFFSET;
        }       
    }
    /*set rhe cnt_result buffer*/
    tempUnit->fifo_results.cnt_result = (SOC_TMC_CNT_RESULT*) &(baseCounter[counters]);


    tempUnit->cacheLock = sal_mutex_create("dpp cache interlock"); 
    if (!tempUnit->cacheLock) {
        LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unable to create unit %d cache"" interlock mutex\n"),unit));
        result = BCM_E_INTERNAL;
        BCMDNX_IF_ERR_EXIT(result);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d warmboot preparations\n"), unit));
        /* now configure the hardware, and coordinate the settings */
        result = _bcm_dpp_wb_counters_state_init(unit, tempUnit);
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d failed warmboot init: %d, %s\n"),
                                       unit, result, _SHR_ERRMSG(result)));
            BCMDNX_IF_ERR_EXIT(result);
        } else {
            wbInit = TRUE;
        }
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */

    for (index = 0; (index < tempUnit->num_counter_procs); index++) {
        sal_memset(entries_bmaps, 0 , sizeof(uint32)*SOC_TMC_CNT_BMAP_OFFSET_COUNT);
        if (!SOC_WARM_BOOT(unit)) {
            SOC_TMC_CNT_COUNTERS_INFO_clear(&(tempUnit->proc[index].mode));
            SOC_TMC_CNT_MODE_EG_clear(&(tempUnit->proc[index].mode.mode_eg));
            /* get counter processor source */
            /* all but last default to ingress PP, last to egress PP */
            result = _soc_dpp_str_prop_parse_counter_source(unit, index, &src_core, &src_type, &command_id, &eg_type, &we_val, &valid);
            BCMDNX_IF_ERR_EXIT(result);
            if (!valid) {
                continue;
            }
            result = _soc_dpp_str_prop_parse_counter_format(unit, index, &(tempUnit->proc[index].mode.format));
            if (result != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter %d failed to read counter format property. processors init failed\n"),unit, index));
                BCMDNX_IF_ERR_EXIT(result);
            }

            SOC_TMC_CNT_VOQ_PARAMS_clear(&voq_cnt);
            if (src_type == SOC_TMC_CNT_SRC_TYPE_VOQ) {
                result = _soc_dpp_str_prop_parse_counter_voq(
                           tempUnit,
                           index,
                           src_type,
                           lastVoqMin,
                           &lastVoqSize,
                           &voq_cnt);
                BCMDNX_IF_ERR_EXIT(result);
            }
            if (src_type == SOC_TMC_CNT_SRC_TYPE_STAG) {
                result = _soc_dpp_str_prop_parse_counter_stag(tempUnit, index, src_type, &stag_lsb);
                BCMDNX_IF_ERR_EXIT(result);
            }

            /*config mode by SOC properties*/
            result = _soc_dpp_str_prop_parse_counter_mode(unit, index, src_type, &stat);
            BCMDNX_IF_ERR_EXIT(result);

            /*get the counters bmap from soc properties*/
            if (stat == SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS) {
                result = _soc_dpp_str_prop_parse_counter_mapping(unit, index, &(tempUnit->proc[index].mode.custom_mode_params.nof_counters), tempUnit->proc[index].mode.custom_mode_params.entries_bmaps);
                BCMDNX_IF_ERR_EXIT(result);
            }
        } else { /*SOC_WARM_BOOT(unit)*/
            stat = tempUnit->proc[index].mode.mode_statistics;
            src_type = tempUnit->proc[index].mode.src_type;
            eg_type = tempUnit->proc[index].mode.mode_eg.type;
            voq_cnt = tempUnit->proc[index].mode.voq_cnt;
            command_id = tempUnit->proc[index].mode.command_id;
            we_val = tempUnit->proc[index].mode.we_val;
            if (src_type == SOC_TMC_CNT_SRC_TYPE_STAG) {
                result = _soc_dpp_str_prop_parse_counter_stag(tempUnit, index, src_type, &stag_lsb);
                BCMDNX_IF_ERR_EXIT(result);
            }
        }
        result = _bcm_dpp_counter_source_config(tempUnit, index, src_core, src_type, command_id, eg_type, we_val, &voq_cnt, stag_lsb);
        BCMDNX_IF_ERR_EXIT(result);

        result = _bcm_dpp_counter_mode_config(tempUnit, index, stat);
        BCMDNX_IF_ERR_EXIT(result);

        result = _bcm_dpp_counter_proc_inUse_config(tempUnit, index);
        BCMDNX_IF_ERR_EXIT(result);

        tempUnit->proc[index].valid = TRUE;
    }

    
    for (index = 0; (index < tempUnit->num_counter_procs); index++) {
        result = _soc_dpp_str_prop_parse_replicated_pkts(unit, index, &(tempUnit->proc[index].mode.replicated_pkts));
        if (result != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d counter %d failed to read counter replicated packets property. processors init failed\n"),
                       unit, index));
                BCMDNX_IF_ERR_EXIT(result);
        }
    } /* for (all counter processors as long as no errors) */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /* Verify conflicts in setting */
        for (index = 0; (index < tempUnit->num_counter_procs); index++) {
            result = _bcm_dpp_counter_unit_state_verify(tempUnit, index);
            BCMDNX_IF_ERR_EXIT(result);
        }
        /* check oam configuration is valid*/
        result = _bcm_dpp_counter_oam_verify(tempUnit);
        BCMDNX_IF_ERR_EXIT(result);

        result = bcm_dpp_statistic_interface_config(unit);
        BCMDNX_IF_ERR_EXIT(result);
    }
#endif
    if (!SOC_WARM_BOOT(unit)) {
        /* only modify hardware state if not warm boot */
        for (index = 0; (index < tempUnit->num_counter_procs); index++) {
            if (!tempUnit->proc[index].valid) {
                continue;
            }
            /* configure the counter processor */
            LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"configure unit %d counter proc %u hardware\n"),unit,index));
            soc_sandRes = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_counters_set,(unit, tempUnit->proc[index].proc_id, &(tempUnit->proc[index].mode))));
            result = handle_sand_result(soc_sandRes);
            BCMDNX_IF_ERR_EXIT(result);
        } /* for (all counter processors as long as no errors) */

#ifdef BCM_WARM_BOOT_SUPPORT
        LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d warmboot preparations\n"), unit));
        /* now configure the hardware, and coordinate the settings */
        result = _bcm_dpp_wb_counters_state_init(unit, tempUnit);
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d failed warmboot init: %d, %s\n"),
                                       unit, result, _SHR_ERRMSG(result)));
            BCMDNX_IF_ERR_EXIT(result);
        } else {
            wbInit = TRUE;
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /*reinit the background thread*/
        if(tempUnit->running == TRUE){
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d counter background thread reinit\n"),
                       unit));
            tempUnit->running = FALSE; /* reset the flag and reinit*/
            result = _bcm_dpp_counter_dma_attach(unit);
            BCMDNX_IF_ERR_EXIT(result);
            
            result = _bcm_dpp_counter_bg_thread_attach(tempUnit);
            BCMDNX_IF_ERR_EXIT(result);
            
        }
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
    if (!SOC_WARM_BOOT(unit)) {
        /*DMA attach in cold boot called in counters_set*/
         uint32 start_bg_thread;

        start_bg_thread = soc_property_get(unit, spn_COUNTER_ENGINE_SAMPLING_INTERVAL, 1);
        tempUnit->running = FALSE;
        tempUnit->bgSem = NULL;
        tempUnit->bgWait = _BACKGROUND_THREAD_ITER_DEFAULT(tempUnit->unit);
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
        result = _bcm_dpp_wb_counters_update_bgWait_state(tempUnit->unit);
        if(result != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d error updating warm boot data: bgWait  \n"),tempUnit->unit));
            BCMDNX_IF_ERR_EXIT(result);
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
        if(start_bg_thread) {
            LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter background thread init\n"),unit));
            result = _bcm_dpp_counter_bg_thread_attach(tempUnit);
            BCMDNX_IF_ERR_EXIT(result);
        }
    }
        /* all went well; attach this unit */
    _bcm_dpp_counter_state[unit] = tempUnit;
    LOG_DEBUG(BSL_LS_BCM_CNT,
              (BSL_META_U(unit,
                          "unit %d counter processors ready\n"),
               unit));

exit:
    if (BCM_E_NONE != result) {
        /* something went wrong */
        LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processors init failed\n"),unit));
        
        /* coverity [check_after_deref : FALSE ] */
        if (tempUnit) {
            for (index = 0; index < tempUnit->num_counter_procs; index++) {
                _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, index);
                if (is_alloc) {
                    _bcm_dpp_wb_counters_proc_inUse_free(unit, index);
                }
            }
            /* destroy the background thread */
            _bcm_dpp_counter_bg_thread_detach(tempUnit);
            if (tempUnit->cacheLock) {
                sal_mutex_destroy(tempUnit->cacheLock);
            }
            /* poison the struct */
            sal_memset(tempUnit, 0x00, sizeof(*tempUnit));
            /* release the struct */
           _bcm_dpp_wb_counters_dealloc(unit);
           BCM_FREE(tempUnit);
        } /* if (tempUnit) */
#ifdef BCM_WARM_BOOT_SUPPORT
        if (wbInit) {
            result = _bcm_dpp_wb_counters_state_deinit(unit);
            if (result != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d, failed warmboot deinit, error 0x%x\n"),
                                      unit,
                           result));
                BCMDNX_IF_ERR_EXIT(result);
            }
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
    } /* if (BCM_E_NONE == result) */
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_counter_avail_get(int unit,
                          unsigned int proc,
                          bcm_dpp_counter_set_t *avail,
                          bcm_dpp_counter_set_t *native)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("invalid counter processor ID %u"),
                          proc));
    }
    if (avail) {
        uint32 i;
        SHR_BITCLR_RANGE(*avail , 0, bcm_dpp_counter_count);
        for( i = 0; i < bcm_dpp_counter_count; i++){
            if(unitData->proc[proc].avail_offsets[i] != UNSUPPORTED_COUNTER_OFFSET){
                SHR_BITSET(*avail , i);
            }
        }
    }
    if (native) {
        sal_memcpy(native, &(unitData->proc[proc].native), sizeof(*native));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_get(int unit,
                         unsigned int proc,
                         unsigned int set_id,
                         bcm_dpp_counter_t type,
                         uint64 *stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   FALSE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_get_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 *stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   TRUE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_set(int unit,
                    unsigned int proc,
                    unsigned int set_id,
                    bcm_dpp_counter_t type,
                    uint64 stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   FALSE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   &stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_set_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   TRUE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   &stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_get(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          uint64 *stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   FALSE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_get_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 uint64 *stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   TRUE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_set(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          /*const*/ uint64 *stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   FALSE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_set_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 /*const*/ uint64 *stat)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unitData,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   TRUE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_diag_info_get(int unit,
                              bcm_dpp_counter_diag_info_t info,
                              unsigned int proc,
                              unsigned int *value)
{
    _bcm_dpp_counter_state_t *unitData;
    uint8 is_alloc;
    unsigned int allocated;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("counter processor %u is not valid"),
                          proc));
    }
    if (!value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory out arg must not be NULL")));
    }
    switch (info) {
    /* global */
    case bcm_dpp_counter_diag_info_num_proc:
        *value = unitData->num_counter_procs;
        break;
    case bcm_dpp_counter_diag_info_bg_active:
        *value = unitData->background_active;
        break;
    case bcm_dpp_counter_diag_info_bg_suspend:
        *value = unitData->background_disable;
        break;
    case bcm_dpp_counter_diag_info_bg_defer:
        *value = unitData->background_defer;
        break;
    case bcm_dpp_counter_diag_info_bg_wait:
        *value = unitData->bgWait;
        break;
    case bcm_dpp_counter_diag_info_fg_hit:
        *value = unitData->foreground_hit;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_background:
        *value = unitData->fifo_read_background;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_deferred:
        *value = unitData->fifo_read_deferred;
        break;
    /* per processor */
    case bcm_dpp_counter_diag_info_source:
        *value = unitData->proc[proc].source;
        break;
    case bcm_dpp_counter_diag_info_stat_tag_low_bit:
        if (SOC_TMC_CNT_SRC_TYPE_STAG == unitData->proc[proc].source) {
            *value = unitData->proc[proc].mode.stag_lsb;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG("stat_tag_low_bit not supported for"
                                       " processor %u source %s"),
                              proc,
                              SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[proc].source)));
        }
        break;
    case bcm_dpp_counter_diag_info_voq_base:
        if (SOC_TMC_CNT_SRC_TYPE_VOQ == unitData->proc[proc].source) {
            *value = unitData->proc[proc].mode.voq_cnt.start_q;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG("voq_base not supported for"
                                       " processor %u source %s"),
                              proc,
                              SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[proc].source)));
        }
        break;
    case bcm_dpp_counter_diag_info_voq_per_set:
        if (SOC_TMC_CNT_SRC_TYPE_VOQ == unitData->proc[proc].source) {
            *value = unitData->proc[proc].mode.voq_cnt.q_set_size;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG("voq_per_set not supported for"
                                       " processor %u source %s"),
                              proc,
                              SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[proc].source)));
        }
        break;
    case bcm_dpp_counter_diag_info_fifo_read_passes:
        *value = unitData->proc[proc].fifo_read_passes;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_fails:
        *value = unitData->proc[proc].fifo_read_fails;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_items:
        *value = unitData->proc[proc].fifo_read_items;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_max:
        *value = unitData->proc[proc].fifo_read_max;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_last:
        *value = unitData->proc[proc].fifo_read_last;
        break;
    case bcm_dpp_counter_diag_info_direct_read_passes:
        *value = unitData->proc[proc].direct_read_passes;
        break;
    case bcm_dpp_counter_diag_info_direct_read_fails:
        *value = unitData->proc[proc].direct_read_fails;
        break;
    case bcm_dpp_counter_diag_info_cache_updates:
        *value = unitData->proc[proc].cache_updates;
        break;
    case bcm_dpp_counter_diag_info_cache_reads:
        *value = unitData->proc[proc].cache_reads;
        break;
    case bcm_dpp_counter_diag_info_cache_writes:
        *value = unitData->proc[proc].cache_writes;
        break;
    case bcm_dpp_counter_diag_info_api_reads:
        *value = unitData->proc[proc].api_reads;
        break;
    case bcm_dpp_counter_diag_info_api_writes:
        *value = unitData->proc[proc].api_writes;
        break;
    case bcm_dpp_counter_diag_info_api_miss_reads:
        *value = unitData->proc[proc].api_miss_reads;
        break;
    case bcm_dpp_counter_diag_info_api_miss_writes:
        *value = unitData->proc[proc].api_miss_writes;
        break;
    case bcm_dpp_counter_diag_info_format:
        *value = unitData->proc[proc].mode.mode_statistics;
        break;
    case bcm_dpp_counter_diag_info_counters:
        *value = unitData->proc[proc].num_counters;
        break;
    case bcm_dpp_counter_diag_info_sets:
        *value = unitData->proc[proc].num_sets;
        break;
    case bcm_dpp_counter_diag_info_alloc_local:
        _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_alloc, proc);
        *value = is_alloc;
        break;
    case bcm_dpp_counter_diag_info_alloc_inuse:
        _bcm_dpp_wb_counters_proc_allocated_get(unit,&allocated, proc);
        *value = allocated;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("invalid info item %d"),
                          info));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_counter_match_stat_dp (int unit, 
                                SOC_TMC_CNT_MODE_STATISTICS proc_stat, 
                                bcm_dpp_counter_t ctrStat, 
                                unsigned int dp, 
                                unsigned int *match,
                                unsigned int *use_color /* If True, the color of stat was used */
                                )
{

    /*the all no color is blocked in the init because there are missing enum value for yellow1, yellow2 and red */
    switch (proc_stat) {
    case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR: 
    case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR: 
    case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR:
    case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR:
        *use_color = 1;
             switch (ctrStat) {
             case bcm_dpp_counter_green_pkts:
             case bcm_dpp_counter_green_bytes:
             case bcm_dpp_counter_fwd_green_bytes:
             case bcm_dpp_counter_fwd_green_pkts:
             case bcm_dpp_counter_drop_green_pkts:
             case bcm_dpp_counter_drop_green_bytes:
                 /* Green only */
                 *match = (dp == 0);
                 break;
             case bcm_dpp_counter_yellow_pkts:
             case bcm_dpp_counter_yellow_bytes:
             case bcm_dpp_counter_fwd_yellow_pkts:
             case bcm_dpp_counter_fwd_yellow_bytes:
             case bcm_dpp_counter_drop_yellow_pkts:
             case bcm_dpp_counter_drop_yellow_bytes:
                 *match = (dp == 1) || (dp == 2);
                 break;
             case bcm_dpp_counter_red_pkts:
             case bcm_dpp_counter_red_bytes:
             case bcm_dpp_counter_fwd_red_pkts:
             case bcm_dpp_counter_fwd_red_bytes:
             case bcm_dpp_counter_drop_red_pkts:
             case bcm_dpp_counter_drop_red_bytes:

                 *match = (dp == 3);
                 break;
             case bcm_dpp_counter_not_green_pkts:
             case bcm_dpp_counter_not_green_bytes:
             case bcm_dpp_counter_fwd_not_green_pkts:
             case bcm_dpp_counter_fwd_not_green_bytes:
             case bcm_dpp_counter_drop_not_green_pkts:
             case bcm_dpp_counter_drop_not_green_bytes:
                 *match = (dp != 0);
                 break;
             case bcm_dpp_counter_drop_bytes:
             case bcm_dpp_counter_drop_pkts:
             case bcm_dpp_counter_fwd_bytes:
             case bcm_dpp_counter_fwd_pkts:
             case bcm_dpp_counter_bytes:
             case bcm_dpp_counter_pkts:
                 *match = 1;
                 break;
             default:
                 *match = 0;
                 break;
             }
             
             break;
    default:
        *match = 0;
        *use_color = 0;
        break;
    }
}

int
bcm_dpp_counter_find_egress_que(int unit,
                         unsigned int max_set_cnt,
                         unsigned int offset,
                         int core_id,
                         unsigned int tm_port,
                         unsigned int is_mc,
                         unsigned int pp_port,
                         bcm_dpp_counter_t ctrStat,
                         unsigned int *use_color, /* If True, the color of stat was used */
                         unsigned int *proc,
                         unsigned int *set_array,
                         unsigned int *set_cnt)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lval, dp, cosq;
    unsigned int index, match;
    unsigned int set_index, system_mc;
    unsigned int counters_in_set;
    int result = BCM_E_NOT_FOUND;
    uint32  out_port_field = 0;
    uint32  base_q_pair = 0;
#ifdef  BCM_ARAD_SUPPORT   
    uint32 soc_sand_rc;
#endif
    soc_error_t soc_result;
    uint32 nof_priorities;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);

    
    for (index = 0; index < unitData->num_counter_procs; index++) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counters_per_set(unitData, index, &counters_in_set));
        if ((unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM || 
             unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT) && unitData->proc[index].mode.src_core == core_id) {
            if(counters_in_set >= 2){
                if((!is_mc) && ((unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_B) ||
                                 (unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_D))){
                    continue;
                }
                if((is_mc) && ((unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_A) ||
                                 (unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_C))){
                    continue;
                }
            }
               
            set_index = 0;
            if (proc) {
                *proc = index;
            }

#ifdef  BCM_ARAD_SUPPORT   
            /* Get the Base-Queue-Pair */
            if (unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM && (SOC_IS_ARAD(unit) || SOC_IS_JERICHO(unit))) {
                soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_dsp_pp_to_base_q_pair_get, (unit, core_id, tm_port, &base_q_pair));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rc); 
            }
#endif

            /* Choose out_port according to queue pair or pp port */
            if (unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP) 
            {
                out_port_field = unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM ?
                    base_q_pair + (offset & 0x7) : pp_port;
                /* 
                 * EGQ TM Macro build 
                 */ 
                
                for (dp = 0; dp <= 3; dp++) {
                    _bcm_dpp_counter_match_stat_dp(unit, unitData->proc[index].mode.mode_statistics, ctrStat, dp, &match, use_color);
                    if (match) {                            
                        lval =  ( (out_port_field & 0xff) << 6  ) | ((offset & 0x7) << 3 ) | ((dp & 0x3) << 1) | (is_mc & 0x1) ;
                        set_array[set_index] = lval;
                        set_index++;
                    }
                }
                
            } else {
                /* 
                 * EPNI TM Macro build 
                 */ 
                /* if system_mc :loop over all entries and create with mc bit (14) =  0/1
                otherwise only entries with mc=0 are created. */
                for (system_mc = 0; system_mc <= 1; system_mc++) {
                    for (dp = 0; dp <= 3; dp++) {
                        _bcm_dpp_counter_match_stat_dp(unit, unitData->proc[index].mode.mode_statistics, ctrStat, dp, &match, use_color);
                        if (match) {
                            if (unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM) {
                                soc_result = soc_port_sw_db_local_to_out_port_priority_get(unit, tm_port, &nof_priorities); 
                                SOCDNX_IF_ERR_EXIT(soc_result);

                                for (cosq = 0 ; cosq < nof_priorities; cosq++) {
                                    out_port_field = base_q_pair + cosq;

                                    lval = ((out_port_field & 0xff) << 5) | ((offset & 0x7) << 2) | (dp & 0x3);
                                    if(counters_in_set == 1){
                                        lval |= ((is_mc & 0x1) << 14);
                                    }
                                    lval |= (system_mc << 13);
                                    set_array[set_index] = lval;
                                    set_index++;
                                }
                            } else {
                                out_port_field = pp_port;

                                lval = ((out_port_field & 0xff) << 5) | ((offset & 0x7) << 2) | (dp & 0x3);
                                if(counters_in_set == 1){
                                    lval |= ((is_mc & 0x1) << 14);
                                }
                                lval |= (system_mc << 13);
                                set_array[set_index] = lval;
                                set_index++;
                            }
                        }
                    }
                }
            }
            /*not matched - return error*/
            if(set_index == 0){
                break;
            }
            if (set_cnt) {
               *set_cnt = set_index;
            }
            result = BCM_E_NONE;
            break;
        }
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_voq(int unit,
                         int core_id,
                         unsigned int voq,
                         unsigned int *proc,
                         unsigned int *set,
                         bcm_dpp_counter_t ctrStat,
                         unsigned int *first,
                         unsigned int *last)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lfirst;
    unsigned int lcount;
    unsigned int lscale;
    unsigned int lindex;
    unsigned int index;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    
    for (index = 0; index < unitData->num_counter_procs; index++) {
        if (SOC_TMC_CNT_SRC_TYPE_VOQ == unitData->proc[index].source && unitData->proc[index].mode.src_core == core_id) {
            lfirst = unitData->proc[index].mode.voq_cnt.start_q;
            lscale = unitData->proc[index].mode.voq_cnt.q_set_size;
            lcount = lscale * unitData->proc[index].num_sets;
            if ((voq >= lfirst) && (voq < (lfirst + lcount))) {
                /* Check also the Counter type matches, except for bcm_dpp_counter_count: bypass for enable_set/get */
                if ((ctrStat == bcm_dpp_counter_count)
                    || SHR_BITGET(unitData->proc[index].native, ctrStat) || unitData->proc[index].avail_offsets[ctrStat] != UNSUPPORTED_COUNTER_OFFSET
                     ) {
                    /* found it */
                    lindex = (voq - lfirst) / lscale;
                    if (proc) {
                        *proc = index;
                    }
                    if (set) {
                        *set = lindex;
                    }
                    if (first) {
                        *first = (lindex * lscale) + lfirst;
                    }
                    if (last) {
                        *last = (lindex * lscale) + (lscale - 1);
                    }
                    result = BCM_E_NONE;
                    break;
                }
            } /* if ((voq >= lfirst) && (voc < (lfirst + lcount)) */
        } /* if (proc is sourced from ingress voq) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_stag(int unit,
                          unsigned int stag,
                          unsigned int *proc,
                          unsigned int *set,
                          unsigned int *first,
                          unsigned int *last)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lfirst;
    unsigned int lcount;
    unsigned int lscale;
    unsigned int lindex;
    unsigned int index;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    
    for (index = 0; index < unitData->num_counter_procs; index++) {
        if (SOC_TMC_CNT_SRC_TYPE_STAG == unitData->proc[index].source) {
            lfirst = 0; 
            lscale = 1 << unitData->proc[index].mode.stag_lsb;
            lcount = lscale * unitData->proc[index].num_sets;
            if ((stag >= lfirst) && (stag < (lfirst + lcount))) {
                /* found it */
                lindex = (stag - lfirst) / lscale;
                if (proc) {
                    *proc = index;
                }
                if (set) {
                    *set = lindex;
                }
                if (first) {
                    *first = (lindex * lscale) + lfirst;
                }
                if (last) {
                    *last = (lindex * lscale) + (lscale - 1);
                }
                result = BCM_E_NONE;
                break;
            } /* if ((voq >= lfirst) && (voc < (lfirst + lcount)) */
        } /* if (proc is sourced from ingress voq) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_vsi(int unit,
                         unsigned int vsi,
                         unsigned int *proc,
                         unsigned int *set)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int index;
    unsigned int lfirst;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    
    for (index = 0; index < unitData->num_counter_procs; index++) {
        /*if (_bcm_dpp_counter_source_egress_vsi == unitData->proc[index].source) {*/
        if ((SOC_TMC_CNT_SRC_TYPE_EGR_PP == unitData->proc[index].source) 
             && 
             ( unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_VSI)) {
            lfirst = unitData->proc[index].mode.mode_eg.base_val;
            if ((vsi >= lfirst) &&
                (vsi < (lfirst + unitData->proc[index].num_sets))) {
                /* found it */
                if (proc) {
                    *proc = index;
                }
                if (set) {
                    *set = vsi - lfirst;
                }
                result = BCM_E_NONE;
                break;
            } /* if (vsi is in range supported by this processor) */
        } /* if (proc is sourced from egress vsi) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_outlif(int unit,
                            unsigned int outlif,
                            unsigned int *proc,
                            unsigned int *set)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lfirst;
    unsigned int index;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    
    for (index = 0; index < unitData->num_counter_procs; index++) {
        /*if (_bcm_dpp_counter_source_egress_outlif == unitData->proc[index].source) {*/
        if ((SOC_TMC_CNT_SRC_TYPE_EGR_PP == unitData->proc[index].source) 
             && 
             ( unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF)) {
            lfirst = unitData->proc[index].mode.mode_eg.base_val;
            if ((outlif >= lfirst) &&
                (outlif < (lfirst + unitData->proc[index].num_sets))) {
                /* found it */
                if (proc) {
                    *proc = index;
                }
                if (set) {
                    *set = outlif - lfirst;
                }
                result = BCM_E_NONE;
                break;
            } /* if (outlif is in range supported by this processor) */
        } /* if (proc is sourced from egress outlif) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_alloc(int unit,
                      uint32 flags,
                      _bcm_dpp_counter_source_t source,
                      unsigned int *proc,
                      unsigned int *set)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int xProc = ~0;
    unsigned int xSet = ~0;
    int result;
    int auxRes;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    if (flags & BCM_DPP_COUNTER_WITH_PROC) {
        xProc = *proc;
    }
    if (flags & BCM_DPP_COUNTER_WITH_ID) {
        xProc = *proc;
        xSet = *set;
    }
    result = _bcm_dpp_counter_alloc(unitData, flags, source, &xProc, &xSet);
    if (BCM_E_NONE == result) {
        /* make sure counter set is cleared when allocating */
        result = _bcm_dpp_counter_clear(unitData, xProc, xSet);
        if (BCM_E_NONE == result) {
            *proc = xProc;
            *set = xSet;
        } else {
            auxRes = _bcm_dpp_counter_free(unitData, xProc, xSet);
            BCMDNX_IF_ERR_EXIT(auxRes);
        }
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_free(int unit,
                     unsigned int proc,
                     unsigned int set)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_free(unitData, proc, set));
    /* make sure counter set is cleared after freeing */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_clear(unitData, proc, set));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_bg_enable_set(int unit,
                              int enable)
{
    _bcm_dpp_counter_state_t *unitData;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    if (enable) {
        unitData->background_disable = FALSE;
    } else {
        unitData->background_disable = TRUE;
    }
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_counters_update_background_disable_state(unit);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_bg_enable_get(int unit,
                              int *enable)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    if (!enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory out arg must not be NULL")));
    }
    if (unitData->background_disable) {
        *enable = FALSE;
    } else {
        *enable = TRUE;
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
int
_bcm_dpp_counters_sw_dump(int unit)
{
    uint32                        proc;
#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
    uint32                        cntr;
#endif /* defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE) */
    uint32                        i;
    uint32                        sLimit;
    _bcm_dpp_counter_state_t     *unitData;
#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
    _bcm_dpp_counter_proc_info_t *info;
#endif
    uint32                        inUse;
    uint8                        is_inUse_alloc;
    unsigned int                 allocated;

    BCMDNX_INIT_FUNC_DEFS;

    /* Make sure the counters module is initialized */
    _COUNTER_UNIT_CHECK(unit, unitData);


    LOG_CLI((BSL_META_U(unit,
                        "\nCOUNTERS:")));
    LOG_CLI((BSL_META_U(unit,
                        "\n---------")));

#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
    LOG_CLI((BSL_META_U(unit,
                        "\nbgWait: %d\n"), unitData->bgWait));

    LOG_CLI((BSL_META_U(unit,
                        "\nbackground_disable: %d\n"), unitData->background_disable));

    LOG_CLI((BSL_META_U(unit,
                        "\nfifo_read_background: %u\n"), unitData->fifo_read_background));

    LOG_CLI((BSL_META_U(unit,
                        "\nfifo_read_deferred: %d\n"), unitData->fifo_read_deferred));
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */


    for (proc = 0; proc < unitData->num_counter_procs; proc++) {

        LOG_CLI((BSL_META_U(unit,
                            "\n\nproc: %d\n"), proc));

#if _TRACK_DIAGS_IN_WARMBOOT_SPACE
        info = &(unitData->proc[proc]);

        LOG_CLI((BSL_META_U(unit,
                            "fifo_read_passes:   %u\n"), info->fifo_read_passes));
        LOG_CLI((BSL_META_U(unit,
                            "fifo_read_fails:    %u\n"), info->fifo_read_fails));
        LOG_CLI((BSL_META_U(unit,
                            "fifo_read_items:    %u\n"), info->fifo_read_items));
        LOG_CLI((BSL_META_U(unit,
                            "fifo_read_max:      %u\n"), info->fifo_read_max));
        LOG_CLI((BSL_META_U(unit,
                            "fifo_read_last:     %u\n"), info->fifo_read_last));
        LOG_CLI((BSL_META_U(unit,
                            "direct_read_passes: %u\n"), info->direct_read_passes));
        LOG_CLI((BSL_META_U(unit,
                            "direct_read_fails:  %u\n"), info->direct_read_fails));
        LOG_CLI((BSL_META_U(unit,
                            "cache_updates:      %u\n"), info->cache_updates));
        LOG_CLI((BSL_META_U(unit,
                            "cache_reads:        %u\n"), info->cache_reads));
        LOG_CLI((BSL_META_U(unit,
                            "cache_writes:       %u\n"), info->cache_writes));
        LOG_CLI((BSL_META_U(unit,
                            "api_reads:          %u\n"), info->api_reads));
        LOG_CLI((BSL_META_U(unit,
                            "api_writes:         %u\n"), info->api_writes));
        LOG_CLI((BSL_META_U(unit,
                            "api_miss_reads:     %u\n"), info->api_miss_reads));
        LOG_CLI((BSL_META_U(unit,
                            "api_miss_writes:    %u\n"), info->api_miss_writes));
#endif /* _TRACK_DIAGS_IN_WARMBOOT_SPACE */

        _bcm_dpp_wb_counters_proc_allocated_get(unit, &allocated, proc);
        LOG_CLI((BSL_META_U(unit,
                            "allocated:          %u\n"), allocated));


#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
        for (cntr = 0; cntr < unitData->proc[proc].num_counters; cntr++) {

            for (i = 0; i < _SUB_COUNT; i++) {

                if(0 != unitData->proc[proc].counter[i]) {
                    LOG_CLI((BSL_META_U(unit,
                                        "counter (%05d, %d):   %llu\n"),
                             cntr,
                             i,
                             unitData->proc[proc].counter[(cntr * _SUB_COUNT) + i]));
                }

            }
        }
#endif /* defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE) */
        _bcm_dpp_wb_counters_proc_inUse_is_allocated(unit, &is_inUse_alloc, proc);
        if(is_inUse_alloc) {
            sLimit = (unitData->proc[proc].num_sets + 7) >> 3;

            for (i = 0; i < sLimit; i++) {
                _bcm_dpp_wb_counters_proc_inUse_get(unit, &inUse, proc, i);
                if(0 != inUse) {
                    LOG_CLI((BSL_META_U(unit,
                                        "inUse (%05d):   0x%02x\n"),
                             i,
                             inUse));
                }

            }

        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    BCMDNX_FUNC_RETURN;
}
#endif /* def BCM_WARM_BOOT_SUPPORT_SW_DUMP */
