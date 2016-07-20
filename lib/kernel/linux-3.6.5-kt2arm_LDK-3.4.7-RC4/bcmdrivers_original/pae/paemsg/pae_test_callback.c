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


static int arp_change_event(struct notifier_block *this, unsigned long event, void *ptr) 
{
    printk(KERN_ERR "BCMDEVS ARP Table Entry Changed [%s]\n",(char *)ptr);
    return 0;
}

struct notifier_block test_notifier = {
    arp_change_event,   // handler
    NULL,               // parameter is inserted later
    0                   // priority
};

char *parameter = "I am a string";

void pae_register_test_callback(void)
{
    uint8_t ip[4] = {192,168,1,57}; // bcmdev
    char *iface = "eth2.2";        
    if (pae_add_arp_watch_notifier(ip, iface, &test_notifier, parameter))
    {
        printk(KERN_ERR "Interface/IP pair is not in the arp watch table\n");
    }
}
