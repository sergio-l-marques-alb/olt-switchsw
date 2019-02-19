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
#define PAE_ARP_POLL_INTERVAL_SECS (10)

// don't store more entries than we can view
#define MAX_PAE_WATCH_ENTRIES (PAGE_SIZE / (sizeof (struct pae_arp_watch_table_entry) + 10))


struct pae_arp_watch_table_entry
{
    uint8_t    ipv4_address[4];
    uint8_t    mac_address[6];
    uint8_t    dev_name[8];     /* network interface ascii name         */
    void       *notify_data;    /* parameter passed to notify callback  */
    struct atomic_notifier_head callback_chain;
};

/* arp watch related device attributes for sysfs */
extern struct device_attribute dev_attr_pae_arp_watch_add;
extern struct device_attribute dev_attr_pae_arp_watch_remove;
extern struct device_attribute dev_attr_pae_arp_watch_timer;

/* arp watch table */
extern struct pae_arp_watch_table_entry pae_arp_watch_table[];
extern int pae_arp_watch_table_num_entries;
extern unsigned pae_arp_watch_timer;
extern int pae_add_arp_watch(uint8_t *ipv4_address, uint8_t* l2_address, const char *interface);

/* poll function */
extern void pae_watch_arp_tables(int interval_secs);

/* test function */
void pae_add_arp_watch_table_test_entry(void);
