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
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/notifier.h>
#include <net/neighbour.h>
#include <net/arp.h>
#include "pae_addr.h"
#include "pae_shared.h"
#include "pae_cmds.h"
#include "pae_regs.h"
#include "pae_arp.h"

/* poll timer - settable through sysfs */
unsigned pae_arp_watch_timer = PAE_ARP_POLL_INTERVAL_SECS;

static int verbose_debug = 1;
static char broadcast_ll2[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
static char unconfigured_ll2[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

struct pae_arp_watch_table_entry pae_arp_watch_table[MAX_PAE_WATCH_ENTRIES];

int pae_arp_watch_table_num_entries = 0;

void pae_add_arp_watch_table_test_entry(void)
{
    uint8_t ip[4] = {192, 168, 1, 57}; // bcmdev
    // uint8_t mac[6] = {0x78, 0x2b, 0xcb, 0x91, 0x6b, 0xe7};
    uint8_t mac[6] = {0x78, 0x2b, 0xcb, 0x00, 0x00, 0xe7};
    char *intf = "eth2.2";
    memcpy(&pae_arp_watch_table[0].ipv4_address, ip, 4);
    memcpy(&pae_arp_watch_table[0].mac_address, mac, 6);
    strncpy(pae_arp_watch_table[0].dev_name, intf, strlen(intf));
    memset(&pae_arp_watch_table[0].callback_chain, 0, sizeof(struct atomic_notifier_head));
    pae_arp_watch_table_num_entries++;
}

/* 
 * periodically send arp requests for ip addresses that are being routed through the PAE
 * instead of the host in case the l2 (MAC) address for the destination should change
 */
void pae_watch_arp_tables(int interval_secs)
{
    int i,rc;
    static int counter = 0;
    static int warned = 0;
    struct neighbour *n;
    struct net_device *dev;

    /* carry on, but warn the user about timer granularity */
    if ((pae_arp_watch_table_num_entries) && (interval_secs > pae_arp_watch_timer) && (!warned)) {
        printk(KERN_INFO "%s: PAE poll interval is too long",__func__);
        warned++;
    }

    counter += interval_secs;

    if (counter > pae_arp_watch_timer) {
        counter = 0;

        for (i=0; i < pae_arp_watch_table_num_entries; i++) {
            dev = dev_get_by_name(&init_net,pae_arp_watch_table[i].dev_name);
            if (dev <= 0) {
                printk(KERN_INFO "%s: invalid device in PAE watch table [%s]\n",__func__,pae_arp_watch_table[i].dev_name);
                continue;
            }
            n = neigh_lookup(&arp_tbl,&pae_arp_watch_table[i].ipv4_address,
                dev_get_by_name(&init_net,pae_arp_watch_table[i].dev_name));
            dev_put(dev);
            if (n) {
                /* force an arp request */
                n->ops->solicit(n,NULL);
                if (verbose_debug) {
                    printk(KERN_DEBUG "%s: neigh lookup of table entry %d returned nud state of %d\n",__func__,i,n->nud_state);
                }
            
                /* 
                 * check if the mac address of this entry has changed to an address other than the broadcast address
                 */

                if ((memcmp(n->ha,broadcast_ll2,sizeof(broadcast_ll2)) != 0) &&
                    (memcmp(n->ha,unconfigured_ll2,sizeof(unconfigured_ll2)) != 0) &&
                    (memcmp(n->ha,pae_arp_watch_table[i].mac_address,sizeof(pae_arp_watch_table[i].mac_address)) != 0)) { 

                    if (verbose_debug) {
                        printk(KERN_DEBUG "Neigbor ha for %d.%d.%d.%d changed to %02x:%02x:%02x:%02x:%02x:%02x\n",
                            pae_arp_watch_table[i].ipv4_address[0],
                            pae_arp_watch_table[i].ipv4_address[1],
                            pae_arp_watch_table[i].ipv4_address[2],
                            pae_arp_watch_table[i].ipv4_address[3],
                            n->ha[0], n->ha[1], n->ha[2],
                            n->ha[3], n->ha[4], n->ha[5]);

                        printk(KERN_DEBUG "it used to be : %02x:%02x:%02x:%02x:%02x:%02x\n",
                            pae_arp_watch_table[i].mac_address[0], pae_arp_watch_table[i].mac_address[1],
                            pae_arp_watch_table[i].mac_address[2], pae_arp_watch_table[i].mac_address[3],
                            pae_arp_watch_table[i].mac_address[4], pae_arp_watch_table[i].mac_address[5]);
                    }
                
                    /* update our table with the new value */
                    memcpy(pae_arp_watch_table[i].mac_address,n->ha,sizeof(pae_arp_watch_table[i].mac_address));

                    /* execute the notification call chain */
                    atomic_notifier_call_chain(&pae_arp_watch_table[i].callback_chain,0,pae_arp_watch_table[i].notify_data);
                }
            } else {
                if (verbose_debug) {
                    printk(KERN_DEBUG "%s: neigh lookup of table entry %d returned NULL... creating entry\n",__func__,i);
                }
                n = __neigh_create(&arp_tbl,pae_arp_watch_table[i].ipv4_address,
                    dev_get_by_name(&init_net,pae_arp_watch_table[i].dev_name),0);
                if (n <= 0) {
                    printk(KERN_INFO "%s: neigh_create returned error code %d\n",__func__,(int)n);
                }
                /* set to incomplete with broadcast l2 address */
                rc = neigh_update(n,broadcast_ll2,NUD_PROBE,NEIGH_UPDATE_F_ADMIN);
                if (rc < 0) {
                    printk(KERN_INFO "%s: neigh_update returned error code %d\n",__func__,rc);
                }
            }
        }
    }
}
