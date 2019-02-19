/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/spinlock.h>

#include "pae_stats.h"
#include "pae_cmds.h"

typedef struct pae_internal_action_cumulative_stats_s {
    uint64_t bytes;
    uint64_t packets;
    uint64_t last_hit_jiffies_64;
    uint32_t last_hit_time_r5;
} pae_internal_action_cumulative_stats_t;

static pae_internal_action_cumulative_stats_t cumulative_action_stats[PAE_NUM_ACTIONS];

static uint64_t pae_r5_cur_time, pae_r5_idle_time;
static unsigned pae_load_ppm, pae_max_load_ppm;  // units of parts-per-million
static int pae_reset_max_load;

static spinlock_t stats_lock = __SPIN_LOCK_UNLOCKED(stats_lock);

void pae_update_cumulative_times(void)
{
    uint32_t pae_r5_cur_time_32, pae_r5_idle_time_32;

    uint32_t pae_r5_last_cur_time_32 = (uint32_t)pae_r5_cur_time;
    uint32_t pae_r5_last_idle_time_32 = (uint32_t)pae_r5_idle_time;

    uint32_t delta_r5_cur_time, delta_r5_idle_time;

    pae_get_times(&pae_r5_cur_time_32, &pae_r5_idle_time_32);

    /* Assumption: this function gets called often enough that the counters wrap at most once */
    delta_r5_cur_time = pae_r5_cur_time_32 - pae_r5_last_cur_time_32;
    delta_r5_idle_time = pae_r5_idle_time_32 - pae_r5_last_idle_time_32;

    pae_r5_cur_time += (uint64_t)delta_r5_cur_time;
    pae_r5_idle_time += (uint64_t)delta_r5_idle_time;

    if (delta_r5_idle_time > delta_r5_cur_time) {
        /* Race condition in reading vs increment can cause us to see slightly larger
           idle time than elapased time.  Cap the idle time at the elapsed. */
        delta_r5_idle_time = delta_r5_cur_time;
    }

    if (delta_r5_cur_time) {
        uint64_t delta_r5_busy_time_times_1M = (uint64_t)(delta_r5_cur_time - delta_r5_idle_time) * 1000000ULL;

        /* use kernel's 64bit-by-32bit divide macro, rather than GCC intrinsic not included in kernel build */
        /* Note: macro returns the remainder, and modifies the first argument... */
        uint64_t load_ppm_64 = delta_r5_busy_time_times_1M;
        do_div(load_ppm_64, delta_r5_cur_time);
        pae_load_ppm = load_ppm_64;

        if (pae_load_ppm > pae_max_load_ppm || pae_reset_max_load) {
            pae_max_load_ppm = pae_load_ppm;
            pae_reset_max_load = 0;
        }
    }
}


void pae_update_cumulative_action_stats(int idx)
{
    uint32_t cur_time_r5, idle_time_r5;
    unsigned long delta_time_jiffies;
    uint64_t current_jiffies_64 = get_jiffies_64();
    pae_action_stats_t stats;
    uint32_t prev_bytes = (uint32_t)cumulative_action_stats[idx].bytes;
    uint32_t prev_packets = (uint32_t)cumulative_action_stats[idx].packets;

    pae_get_action_stats(idx, &stats);
    pae_get_times(&cur_time_r5, &idle_time_r5);
    
    if (stats.last_hit_time_r5 != cumulative_action_stats[idx].last_hit_time_r5) {
        delta_time_jiffies = (cur_time_r5 - stats.last_hit_time_r5) / (R5_TICKS_PER_JIFFY);
        
        cumulative_action_stats[idx].last_hit_jiffies_64 = current_jiffies_64 - delta_time_jiffies;

        cumulative_action_stats[idx].last_hit_time_r5 = stats.last_hit_time_r5;
    }
    
    if (prev_bytes != stats.bytes) {
        cumulative_action_stats[idx].bytes += (uint64_t)(uint32_t)(stats.bytes - prev_bytes);
    }

    if (prev_packets != stats.packets) {
        cumulative_action_stats[idx].packets += (uint64_t)(uint32_t)(stats.packets - prev_packets);
    }
}

void pae_update_cumulative_statistics(void)
{
    int i;
    
    spin_lock(&stats_lock);
    pae_update_cumulative_times();
    
    for (i = 0; i < PAE_NUM_ACTIONS; ++i) {
        pae_update_cumulative_action_stats(i);
    }    
    spin_unlock(&stats_lock);
}


int pae_get_cumulative_times(uint64_t *cur_time, uint64_t *idle_time)
{
    spin_lock(&stats_lock);
    *cur_time = pae_r5_cur_time;
    *idle_time = pae_r5_idle_time;
    spin_unlock(&stats_lock);

    return 0;
}


int pae_get_cumulative_action_stats(unsigned idx, pae_cumulative_action_stats_t *stats)
{
    spin_lock(&stats_lock);
    stats->bytes = cumulative_action_stats[idx].bytes;
    stats->packets = cumulative_action_stats[idx].packets;
    stats->last_hit_jiffies = cumulative_action_stats[idx].last_hit_jiffies_64;
    spin_unlock(&stats_lock);

    return 0;
}


int pae_get_load(uint32_t *cur_load_ppm, uint32_t *max_load_ppm)
{
    spin_lock(&stats_lock);
    *cur_load_ppm = pae_load_ppm;
    *max_load_ppm = pae_max_load_ppm;
    spin_unlock(&stats_lock);

    return 0;
}


int pae_reset_load(void)
{
    pae_reset_max_load = 1;
    
    return 0;
}
