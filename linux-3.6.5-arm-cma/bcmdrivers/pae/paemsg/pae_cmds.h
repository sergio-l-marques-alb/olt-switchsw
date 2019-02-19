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

#ifndef PAE_CMDS_H
#define PAE_CMDS_H

#include <linux/types.h>
#include <linux/device.h>
#include <linux/jiffies.h>

#include "pae_shared.h"

#define R5_TICKS_PER_JIFFY (500000000 / 16 / HZ)  /* 500MHz, 16 cycles per clock incr */

#define NUM_STATS_ATTRS (3)

typedef struct pae_action_stats_s {
    unsigned long bytes;
    unsigned long packets;
    uint32_t last_hit_time_r5;
} pae_action_stats_t;

/* Function interface */
int pae_feature_supported(u32 feature_mask);

void pae_set_default_mtu(unsigned mtu);
int pae_add_action(unsigned idx, int action_type, u8 *action_data, unsigned action_data_len_bytes);
int pae_add_action_with_mtu(unsigned idx, unsigned action_type, unsigned mtu, u8 *action_data, unsigned action_data_len_bytes);

int pae_delete_action(unsigned idx);
int pae_update_action(unsigned idx, unsigned offset, u8 *updated_data, unsigned update_len_bytes);

int pae_add_tuple_rule(uint8_t ip_version, uint8_t *src_addr, uint8_t *dest_addr, 
    uint16_t src_port, uint16_t dest_port, uint8_t protocol, uint32_t action_idx, uint32_t flags);
int pae_delete_tuple_rule(uint8_t ip_version, uint8_t *src_addr, uint8_t *dest_addr, 
    uint16_t src_port, uint16_t dest_port, uint8_t protocol, uint32_t flags);
int pae_set_tuple_mask(uint8_t ip_version, uint8_t *src_addr, uint8_t *dest_addr, 
    uint16_t src_port, uint16_t dest_port, uint8_t protocol);

/* Get hit counts and last hit time for an action.   */
/*     Values are absolute, and will eventually wrap */
int pae_get_action_stats(unsigned idx, pae_action_stats_t *stats);

int pae_get_times(uint32_t *cur_time, uint32_t *idle_time);

/* low level */
unsigned pae_do_cmd(unsigned cmd);

/* arp watch callback registration */
int pae_add_arp_watch_notifier(uint8_t *ipv4_address, const char *interface, struct notifier_block *nb, void *notify_data);

/* For debug use.  May be removed in future. */
uint32_t pae_read_memory(uint32_t addr);
uint32_t pae_write_memory(uint32_t addr, uint32_t val);

/* SysFS File interface */

typedef struct {
    char name[16];
    struct device_attribute dev_attr;
} dev_attr_action_stats_t;

int pae_add_action_stats_sysfs(int idx);
void pae_remove_action_stats_sysfs(int idx);

extern dev_attr_action_stats_t dev_attr_action_stats[PAE_NUM_ACTIONS][NUM_STATS_ATTRS];
extern struct device_attribute dev_attr_pae_action_add;
extern struct device_attribute dev_attr_pae_action_delete;
extern struct device_attribute dev_attr_pae_action_update;
extern struct device_attribute dev_attr_pae_action_clear_error;
extern struct device_attribute dev_attr_pae_tuple_rule_add;
extern struct device_attribute dev_attr_pae_tuple_rule_delete;
extern struct device_attribute dev_attr_pae_tuple_mask;
extern struct device_attribute dev_attr_pae_idle_time;
extern struct device_attribute dev_attr_pae_cpu_load;

/* For debug: Host can request PAE to read or write memory in its address space */
extern struct device_attribute dev_attr_pae_memory;





#endif /* PAE_CMDS_H */
