/*
 * $Id: counters.h,v 1.23 Broadcom SDK $
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

#ifndef _BCM_INT_DPP_COUNTERS_H_
#define _BCM_INT_DPP_COUNTERS_H_


#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#include <soc/types.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>

/*
 *  Enum for bits in bitmap listing provided stats in current mode, and for the
 *  individual counters within a list of counters to manupulate.
 *
 *  Note that some of these items would be synthetic in various modes, and many
 *  of them will not be supported in all modes.
 *
 *  WARNING: When adding entries here, add both pkts and bytes versions, in
 *  the order as demonstrated.  In several places, it is assumed that one can
 *  add one to a pkts value here to get the same bytes value, or subtract one
 *  from a bytes value to get a pkts value, in this enum.
 *
 *  Some of these values may not be supported.
 */

typedef enum bcm_dpp_counter_e {
    /* forwarded and dropped frames and bytes */
    bcm_dpp_counter_pkts,
    bcm_dpp_counter_bytes,
    bcm_dpp_counter_green_pkts,
    bcm_dpp_counter_green_bytes,
    bcm_dpp_counter_not_green_pkts,
    bcm_dpp_counter_not_green_bytes,
    bcm_dpp_counter_yellow_pkts,
    bcm_dpp_counter_yellow_bytes,
    bcm_dpp_counter_not_yellow_pkts,
    bcm_dpp_counter_not_yellow_bytes,
    bcm_dpp_counter_red_pkts,
    bcm_dpp_counter_red_bytes,
    bcm_dpp_counter_not_red_pkts,
    bcm_dpp_counter_not_red_bytes,
    /* forwarded frames and bytes */
    bcm_dpp_counter_fwd_pkts,
    bcm_dpp_counter_fwd_bytes,
    bcm_dpp_counter_fwd_green_pkts,
    bcm_dpp_counter_fwd_green_bytes,
    bcm_dpp_counter_fwd_not_green_pkts,
    bcm_dpp_counter_fwd_not_green_bytes,
    bcm_dpp_counter_fwd_yellow_pkts,
    bcm_dpp_counter_fwd_yellow_bytes,
    bcm_dpp_counter_fwd_not_yellow_pkts,
    bcm_dpp_counter_fwd_not_yellow_bytes,
    bcm_dpp_counter_fwd_red_pkts,
    bcm_dpp_counter_fwd_red_bytes,
    bcm_dpp_counter_fwd_not_red_pkts,
    bcm_dpp_counter_fwd_not_red_bytes,
    /* discarded frames and bytes */
    bcm_dpp_counter_drop_pkts,
    bcm_dpp_counter_drop_bytes,
    bcm_dpp_counter_drop_green_pkts,
    bcm_dpp_counter_drop_green_bytes,
    bcm_dpp_counter_drop_not_green_pkts,
    bcm_dpp_counter_drop_not_green_bytes,
    bcm_dpp_counter_drop_yellow_pkts,
    bcm_dpp_counter_drop_yellow_bytes,
    bcm_dpp_counter_drop_not_yellow_pkts,
    bcm_dpp_counter_drop_not_yellow_bytes,
    bcm_dpp_counter_drop_red_pkts,
    bcm_dpp_counter_drop_red_bytes,
    bcm_dpp_counter_drop_not_red_pkts,
    bcm_dpp_counter_drop_not_red_bytes,
	/* entry by index */
    bcm_dpp_counter_offset0_pkts,
	bcm_dpp_counter_offset0_bytes,
    bcm_dpp_counter_offset1_pkts,
	bcm_dpp_counter_offset1_bytes,
	bcm_dpp_counter_offset2_pkts,
	bcm_dpp_counter_offset2_bytes,
	bcm_dpp_counter_offset3_pkts,
	bcm_dpp_counter_offset3_bytes,
	bcm_dpp_counter_offset4_pkts,
	bcm_dpp_counter_offset4_bytes,
	bcm_dpp_counter_offset5_pkts,
	bcm_dpp_counter_offset5_bytes,
	bcm_dpp_counter_offset6_pkts,
	bcm_dpp_counter_offset6_bytes,
	bcm_dpp_counter_offset7_pkts,
	bcm_dpp_counter_offset7_bytes,
	/* limit */
    bcm_dpp_counter_count /* NOT A VALID ENTRY; MUST BE LAST */
} bcm_dpp_counter_t;

/* entry bmap- bits mapping*/
#define GREEN_FWD   (1 << SOC_TMC_CNT_BMAP_OFFSET_GREEN_FWD)
#define GREEN_DROP  (1 << SOC_TMC_CNT_BMAP_OFFSET_GREEN_DROP)
#define YELLOW_FWD  (1 << SOC_TMC_CNT_BMAP_OFFSET_YELLOW_FWD)
#define YELLOW_DROP (1 << SOC_TMC_CNT_BMAP_OFFSET_YELLOW_DROP)
#define RED_FWD	    (1 << SOC_TMC_CNT_BMAP_OFFSET_RED_FWD)
#define RED_DROP    (1 << SOC_TMC_CNT_BMAP_OFFSET_RED_DROP)
#define BLACK_FWD   (1 << SOC_TMC_CNT_BMAP_OFFSET_BLACK_FWD)
#define BLACK_DROP  (1 << SOC_TMC_CNT_BMAP_OFFSET_BLACK_DROP)
/*
 *  This is used when querying what stats are available.
 */
typedef SHR_BITDCL bcm_dpp_counter_set_t[_SHR_BITDCLSIZE(bcm_dpp_counter_count)];

/*
 *  Specifies the underlying counter source
 *
 *  This is here only to make debugging easier; it is directly mapped from the
 *  value of the counter_source SOC property.
 */

typedef SOC_TMC_CNT_SRC_TYPE _bcm_dpp_counter_source_t;

/*
 *  Diagnostic information tags
 */
typedef enum bcm_dpp_counter_diag_info_e {
    /* global */
    bcm_dpp_counter_diag_info_num_proc,
    bcm_dpp_counter_diag_info_bg_active,
    bcm_dpp_counter_diag_info_bg_suspend,
    bcm_dpp_counter_diag_info_bg_defer,
    bcm_dpp_counter_diag_info_bg_wait,
    bcm_dpp_counter_diag_info_fg_hit,
    bcm_dpp_counter_diag_info_fifo_read_background,
    bcm_dpp_counter_diag_info_fifo_read_deferred,
    /* per counter processor */
    bcm_dpp_counter_diag_info_fifo_read_passes,
    bcm_dpp_counter_diag_info_fifo_read_fails,
    bcm_dpp_counter_diag_info_fifo_read_items,
    bcm_dpp_counter_diag_info_fifo_read_max,
    bcm_dpp_counter_diag_info_fifo_read_last,
    bcm_dpp_counter_diag_info_direct_read_passes,
    bcm_dpp_counter_diag_info_direct_read_fails,
    bcm_dpp_counter_diag_info_cache_updates,
    bcm_dpp_counter_diag_info_cache_reads,
    bcm_dpp_counter_diag_info_cache_writes,
    bcm_dpp_counter_diag_info_api_reads,
    bcm_dpp_counter_diag_info_api_writes,
    bcm_dpp_counter_diag_info_api_miss_reads,
    bcm_dpp_counter_diag_info_api_miss_writes,
    bcm_dpp_counter_diag_info_source,
    bcm_dpp_counter_diag_info_command_id,
    bcm_dpp_counter_diag_info_stat_tag_low_bit,
    bcm_dpp_counter_diag_info_voq_base,
    bcm_dpp_counter_diag_info_voq_per_set,
    bcm_dpp_counter_diag_info_format,
    bcm_dpp_counter_diag_info_counters,
    bcm_dpp_counter_diag_info_sets,
    bcm_dpp_counter_diag_info_alloc_local,
    bcm_dpp_counter_diag_info_alloc_inuse,
    /* limit */
    bcm_dpp_counter_diag_info_count /* NOT A VALID ENTRY; MUST BE LAST */
} bcm_dpp_counter_diag_info_t;

/*
 *  Counter format
 */
typedef enum bcm_dpp_counter_format_e {
    bcm_dpp_counter_format_packets,
    bcm_dpp_counter_format_bytes,
    bcm_dpp_counter_format_packets_and_bytes,
    bcm_dpp_counter_format_max_q_size
} bcm_dpp_counter_format_t;

/*
 *  Counter format
 */
typedef enum bcm_dpp_counter_replicated_pkts_e {
    bcm_dpp_counter_replicated_pkts_all,
    bcm_dpp_counter_replicated_pkts_frwrd_and_mc,
    bcm_dpp_counter_replicated_pkts_frwrd
} bcm_dpp_counter_replicated_pkts_t;
/*
 *  Flags for allocation of counters
 *
 *  WITH_ID indicates both processor and set ID are specified
 *  WITH_PROC indicates processor ID is specified
 */
#define BCM_DPP_COUNTER_WITH_ID   0x00000001
#define BCM_DPP_COUNTER_WITH_PROC 0x00000002


/*
 *  Subcounter index -- each counter is a group, consisting of sub counters.
 */
#define _SUB_PKTS 0  /* sub counter for frames */
#define _SUB_BYTES 1 /* sub counter for bytes */
#define _SUB_COUNT 2 /* number of sub counters */

/* packets and packets mode Subcounter indexes*/
#define _SUB_FWD_PKTS   _SUB_BYTES /* sub counter for forward packets */
#define _SUB_DROP_PKTS  _SUB_PKTS  /* sub counter for droped packets */


#define UNSUPPORTED_COUNTER_OFFSET (256)

/*
 *  Per counter processor information
 *
 *  The term 'counter pair' here refers to the bytes+frames counter pair per
 *  element in the hardware, not some larger scale pairing, such as the ingress
 *  colour disabled mode; these larger groupings are covered by the format.
 *  The term 'counter set' describes such larger grouping.
 *
 *  Certain sources (such as VOQ or VSI) have implied assignments of the
 *  counter sets; those sources will not allocate any in use bits.  Sources
 *  that do not imply counter set assignments (such as PP or STAG) will
 *  allocate bits for tracking which sets are in use.
 */
typedef struct _bcm_dpp_counter_proc_info_s {
    uint8 valid;
    /* descriptors for driver */
    SOC_TMC_CNT_PROCESSOR_ID proc_id;            /* this counter processor ID */
    SOC_TMC_CNT_COUNTERS_INFO mode;              /* this counter processor mode */
    /* optimisations */
    bcm_dpp_counter_set_t native;           /* supported counters - native */
    /* each entry can be unsupported val - unsupported counter/ offset - for native counter/ offsets bmap - for simulated counter*/
    uint32 avail_offsets[bcm_dpp_counter_count];   
    _bcm_dpp_counter_source_t source;       /* source implied by the mode */
    unsigned int num_counters;              /* number of counter pairs */
    unsigned int num_sets;                  /* number of counter sets */
    /* counter data cache pointer */
    uint64 *counter;                        /* cache of this proc's counters */

    /* statistics and diagnostics */
    
    unsigned int fifo_read_passes;          /* FIFO read successes */
    unsigned int fifo_read_fails;           /* FIFO read errors */
    unsigned int fifo_read_items;           /* total counters in FIFO reads */
    unsigned int fifo_read_max;             /* largest single FIFO read */
    unsigned int fifo_read_last;            /* last counter read by FIFO */
    unsigned int direct_read_passes;        /* direct read successes */
    unsigned int direct_read_fails;         /* direct read errors */
    unsigned int cache_updates;             /* cache updates (by bg/direct) */
    unsigned int cache_reads;               /* cache reads */
    unsigned int cache_writes;              /* cache writes */
    unsigned int api_reads;                 /* API requested reads */
    unsigned int api_writes;                /* API requested writes */
    unsigned int api_miss_reads;            /* API req reads to invalid ctr */
    unsigned int api_miss_writes;           /* SPI req writes to invalid ctr */
    /*unsigned int allocated; */                /* number of allocated sets */
    /*uint8 *inUse; */                         /*  bits indicating in use sets */
} _bcm_dpp_counter_proc_info_t;

typedef struct _bcm_dpp_counter_global_info_s {
    uint8 haveStatTag;
} _bcm_dpp_counter_global_info_t;

/*
 *  Per unit information
 */
typedef struct _bcm_dpp_counter_state_s {
    /* per unit information */
    int unit;                               /* unit number */
    int running;                            /* unit background thread enable */
    sal_thread_t background;                /* unit background thread */
    sal_sem_t bgSem;                        /* unit background waiting sem */
    int bgWait;                             /* delay per bg iter (microsec) */
    unsigned int num_counter_procs;         /* number of counter processors */
    /* interlocking */
    sal_mutex_t cacheLock;                  /* cache access locking */
    int background_defer;                   /* defer background updates */
    int background_active;                  /* background update occurring */
    int background_disable;                 /* background updates disabled */
    int foreground_hit;                     /* a foreground access occurred */
    /* statistics and diagnostics */
    unsigned int fifo_read_background;      /* background FIFO attempts */
    unsigned int fifo_read_deferred;        /* deferred FIFO attempts */
    /* per processor information */
    _bcm_dpp_counter_proc_info_t *proc;     /* per processor information */
    _bcm_dpp_counter_global_info_t global_info;
    SOC_TMC_CNT_RESULT_ARR fifo_results;    /* buffer for the data read from fifo by the background thread*/
} _bcm_dpp_counter_state_t;


extern const char * const bcm_dpp_counter_t_names[];
extern _bcm_dpp_counter_state_t *_bcm_dpp_counter_state[SOC_MAX_NUM_DEVICES];
extern int _bcm_counter_thread_is_running[SOC_MAX_NUM_DEVICES];


/*
 *   Name
 *     bcm_dpp_counter_init
 *   Purpose
 *     Initialise the counter processor handling
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
extern int
bcm_dpp_counter_init(int unit);



/*
 *   Name
 *     bcm_dpp_counter_background_collection_enable_set
 *   Purpose
 *     Stop the background thread and disable DMA FIFO
 *   Arguments
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN int enable = enable/disable counter thread and DMA operation
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
extern int 
bcm_dpp_counter_background_collection_enable_set(int unit, int enable);

/*
 *   Name
 *     bcm_dpp_counter_background_collection_enable_get
 *   Purpose
 *     Get the background collection enable/disable mode
 *   Arguments
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     OUT int enable = counter thread status - enable/disable 
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes:
 *   Not check DMA status
 */
extern int 
bcm_dpp_counter_background_collection_enable_get(int unit, int *enable);

/*
 *   Name
 *     bcm_dpp_counter_detach
 *   Purpose
 *     Deinitialise the counter processor handling
 *   Arguments
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
extern int
bcm_dpp_counter_detach(int unit);

/*
 *   Name
 *     bcm_dpp_counter_avail_get
 *   Purpose
 *     Get information about which stats are provided for a given counter set.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     OUT bcm_dpp_counter_set_t *avail = where to put available set
 *     OUT bcm_dpp_counter_set_t *native = where to put native set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     The 'available' set includes all stats that are available in the current
 *     mode, both native and emulated.
 *
 *     The 'native' set includes all stats that are supported natively by the
 *     current hardware mode.
 *
 *     Here 'native' refers to stats that are directly supported by the current
 *     hardware mode, while 'emulated' refers to stats that can be derived by
 *     combining 'native' stats in some way.  For example, if the current
 *     hardware mode provides {drop green, drop yellow, drop red}, it is
 *     possible to emulate {drop all, drop not green, drop not yellow, drop not
 *     red} from the native information.
 *
 *     In error case, both avail and native will be cleared.
 *
 *     Current and next generation (Soc_petra and Arad) do not support multiple
 *     modes per counter processor, so this only takes counter processor as the
 *     argument to determine the mode.  May change in distant future.
 */
extern int
bcm_dpp_counter_avail_get(int unit,
                          unsigned int proc,
                          bcm_dpp_counter_set_t *avail,
                          bcm_dpp_counter_set_t *native);

/*
 *   Name
 *     bcm_dpp_counter_get(_cached)
 *   Purpose
 *     Retrieve a specific statistic from a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to read
 *     IN bcm_dpp_counter_t type = which stat to retrieve
 *     OUT uint64 *stat = where to put the retrieved stat
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     It is far more efficient to use the multt_get if you want more than one.
 *
 *     May return success when getting stats that are not supported in the
 *     current mode; such stats will be returned as zero.
 *
 *     Some stats may be filled in by emulating them (arithmetic on native
 *     stats).
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_get(int unit,
                    unsigned int proc,
                    unsigned int set_id,
                    bcm_dpp_counter_t type,
                    uint64 *stat);
extern int
bcm_dpp_counter_get_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 *stat);

/*
 *   Name
 *     bcm_dpp_counter_set(_cached)
 *   Purpose
 *     Updates a specific statistic of a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to write
 *     IN bcm_dpp_counter_t type = which stat to set
 *     IN uint64 stat = the new value for the statistic
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     It is far more efficient to use the multt_set if you want more than one.
 *
 *     May return success even for stats that are not supported in the current
 *     mode; no changes will be made for such stats.
 *
 *     For stats that are emulated by arithmetic, the values of the individual
 *     'native' stats participating in an emulated stat are not guaranteed;
 *     only that the total will be as written.  It is therefore best to only
 *     set native stats or to only set stats to zero.
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_set(int unit,
                    unsigned int proc,
                    unsigned int set_id,
                    bcm_dpp_counter_t type,
                    uint64 stat);
extern int
bcm_dpp_counter_set_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 stat);

/*
 *   Name
 *     bcm_dpp_counter_multi_get(_cached)
 *   Purpose
 *     Retrieve specific stats from a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to read
 *     IN unsigned int count = number of stats to retrieve
 *     IN bcm_dpp_counter_t *type = list of stats to retrieve
 *     OUT uint64 *stats = where to put the retrieved stats
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     On success, some stats may not have been supported in the particular
 *     configuration.  These stats will be zeroed.
 *
 *     Some stats may be filled in by emulating them (arithmetic on known
 *     stats).
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     type and stat are pointers to the initial element of respective arrays.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_multi_get(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          uint64 *stat);
extern int
bcm_dpp_counter_multi_get_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 uint64 *stat);

/*
 *   Name
 *     bcm_dpp_counter_multi_set(_cached)
 *   Purpose
 *     Set specific stats from a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to write
 *     IN unsigned int count = number of stats to set
 *     IN bcm_dpp_counter_t *type = list of stats to set
 *     IN uint64 *stats = where to put the retrieved stats
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     On success, stats that are not supported in the particular mode were
 *     ignored (no changes made).
 *
 *     For stats that are emulated by arithmetic, the values of the individual
 *     'native' stats participating in an emulated stat are not guaranteed;
 *     only that the total will be as written.  It is therefore best to only
 *     set native stats or to only set stats to zero.
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     type and stat are pointers to the initial element of respective arrays.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_multi_set(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          /*const*/ uint64 *stat);
extern int
bcm_dpp_counter_multi_set_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 /*const*/ uint64 *stat);

/*
 *   Name
 *     bcm_dpp_counter_diag_info_get
 *   Purpose
 *     DIAGNOSTICS: Retrieve information about the counter state on a unit
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN bcm_dpp_counter_diag_info_t info = which item to retrieve
 *     IN unsigned int proc = the counter processor on which to operate
 *     OUT unsigned int *value = where to put the retrieved value
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Some items apply globally to a unit; others only to a single counter
 *     processor.  For those that are for a particular counter processor, the
 *     processor number chooses the counter processor.  For all cases, the
 *     processor number must be valid, so it is best to use zero as the
 *     processor number for any of the global information.
 */
extern int
bcm_dpp_counter_diag_info_get(int unit,
                              bcm_dpp_counter_diag_info_t info,
                              unsigned int proc,
                              unsigned int *value);
/*
 *   Name
 *     bcm_dpp_counter_find_egress_que
 *   Purpose
 *     Find the counter processor and set that is used for egress.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int max_set_cnt - Size of result array
 *     IN unsigned int offset = Offset (TC)
 *     IN unsigned int tm_port
 *     IN unsigned int is_mc
 *     IN unsigned int pp_port
 *     OUT unsigned int *proc = the counter processor handlng the results
 *     OUT unsigned int *set_array = the counter set handling the results
 *     OUT unsigned int *set_cnt = the number of actual entries in set_array
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the VOQ is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
typedef struct bcm_dpp_counter_proc_and_set_s {
    int ctr_proc_id;
    int ctr_set_id;
} bcm_dpp_counter_proc_and_set_t;


extern int
bcm_dpp_counter_find_egress_que(int unit,
                         unsigned int max_set_cnt,
                         unsigned int offset,
                         int core_id,
                         unsigned int tm_port,
                         unsigned int system_mc,
                         unsigned int is_mc,
                         unsigned int pp_port,
                         bcm_dpp_counter_t ctrStat,
                         unsigned int *use_color, /* If True, the color of stat was used */
                         bcm_dpp_counter_proc_and_set_t *proc_set_array,
                         unsigned int *set_cnt);
 
/*
 *   Name
 *     bcm_dpp_counter_find_voq
 *   Purpose
 *     Find the counter processor and set that is used for a paricular VOQ, and
 *     the range of VOQs associated with that processor and set.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int voq = the VOQ of interest
 *     OUT unsigned int *proc = the counter processor handlng the VOQ
 *     OUT unsigned int *set = the counter set handling the VOQ
 *     OUT unsigned int *first = the first VOQ handled by the set
 *     OUT unsigned int *last = the last VOQ handled by the set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the VOQ is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_voq(int unit,
                         int core_id,
                         unsigned int voq,
                         bcm_dpp_counter_proc_and_set_t *proc_and_set,
                         bcm_dpp_counter_t ctrStat,
                         unsigned int *first,
                         unsigned int *last);

/*
 *   Name
 *     bcm_dpp_counter_find_stag
 *   Purpose
 *     Find the counter processor and set that is used for a paricular
 *     statistics tag, and the range of statistics tags associated with that
 *     processor and set.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int stag = the stat tag of interest
 *     OUT unsigned int *proc = the counter processor handlng the stat tag
 *     OUT unsigned int *set = the counter set handling the stat tag
 *     OUT unsigned int *first = the first stat tag handled by the set
 *     OUT unsigned int *last = the last stat tag handled by the set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the stat tag is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_stag(int unit,
                          unsigned int stag,
                          unsigned int *proc,
                          unsigned int *set,
                          unsigned int *first,
                          unsigned int *last);

/*
 *   Name
 *     bcm_dpp_counter_find_vsi
 *   Purpose
 *     Find the counter processor and set that is used for a paricular VSI
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int vsi = the VSI of interest
 *     OUT unsigned int *proc = the counter processor handlng the VSI
 *     OUT unsigned int *set = the counter set handling the VSI
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the VSI is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_vsi(int unit,
                         unsigned int vsi,
                         unsigned int *proc,
                         unsigned int *set);

/*
 *   Name
 *     bcm_dpp_counter_find_outlif
 *   Purpose
 *     Find the counter processor and set that is used for a paricular outlif
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int voq = the outlif of interest
 *     OUT unsigned int *proc = the counter processor handlng the outlif
 *     OUT unsigned int *set = the counter set handling the outlif
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the outlif is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_outlif(int unit,
                            unsigned int outlif,
                            unsigned int *proc,
                            unsigned int *set);

/*
 *  Name
 *    bcm_dpp_counter_alloc
 *  Purpose
 *    Allocate a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN int unit = the unit number on which to operate
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
extern int
bcm_dpp_counter_alloc(int unit,
                      uint32 flags,
                      _bcm_dpp_counter_source_t source,
                      unsigned int *proc,
                      unsigned int *set);

/*
 *  Name
 *    bcm_dpp_counter_free
 *  Purpose
 *    Free a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN int unit = the unit number on which to operate
 *    IN unsigned int proc = the counter processor for the counter set
 *    IN unsigned int set_id = the ID of the counter set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 */
extern int
bcm_dpp_counter_free(int unit,
                     unsigned int proc,
                     unsigned int set);

/*
 *  Name
 *    bcm_dpp_counter_bg_enable_set
 *  Purpose
 *    Set background thread's access to the counter processor.
 *  Arguments
 *    IN int unit = the unit number on which to operate
 *    IN int enable = TRUE to allow background updates, FALSE to disallow
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 *    Disabling background updates does not prevent demand accesses reading the
 *    counter hardware, either directly or through the ejection FIFO.
 */
extern int
bcm_dpp_counter_bg_enable_set(int unit,
                              int enable);

/*
 *  Name
 *    bcm_dpp_counter_bg_enable_get
 *  Purpose
 *    Set background thread's access to the counter processor.
 *  Arguments
 *    IN int unit = the unit number on which to operate
 *    OUT int *enable = TRUE to allow background updates, FALSE to disallow
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 *    Disabling background updates does not prevent demand accesses reading the
 *    counter hardware, either directly or through the ejection FIFO.
 */
extern int
bcm_dpp_counter_bg_enable_get(int unit,
                              int *enable);

extern int
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
        uint32 stag_lsb);
int
bcm_dpp_counter_config_get(
        int unit,
        soc_port_t crps_index,
        uint8* enabled,
        int* src_core,
        SOC_TMC_CNT_SRC_TYPE* src_type,
        int *command_id,
        SOC_TMC_CNT_MODE_EG_TYPE* eg_type,
        SOC_TMC_CNT_FORMAT* format,
        SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val,
        SOC_TMC_CNT_CUSTOM_MODE_PARAMS* custom_mode_params,
        SOC_TMC_CNT_VOQ_PARAMS* voq_cnt,
        uint32* stag_lsb);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_counters_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
         

#endif /* ndef BCM_INT_DPP_COUNTERS_H */

