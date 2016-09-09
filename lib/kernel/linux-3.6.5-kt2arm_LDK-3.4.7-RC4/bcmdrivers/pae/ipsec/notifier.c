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
#include <linux/inetdevice.h>
#include <linux/notifier.h>
#include <net/neighbour.h>
#include <net/net_namespace.h>
#include <linux/seq_file.h>
#include <net/../../net/8021q/vlan.h>
#include <net/arp.h>
#include "ipsec_notify.h"
#include "pae_cmds.h"
#include "pae_ipsec.h"
#include "pae_arp.h"

struct local_if local_if_info[LOCAL_IF_COUNT];
static int PaeIpsecInitialized = 0;

/*------------------- netdev notifier ---------------------*/

struct local_if *PaeHostGetLanInterface(void)
{
	struct local_if *if_entry = NULL;
	int n;

	if (!PaeIpsecInitialized) {
		return NULL;
	}

	for (n = 0; n < LOCAL_IF_COUNT; n++) {
		if_entry = &local_if_info[n];
		if (if_entry->vlan_id && !if_entry->wan) {
			return if_entry;
		}
	}
	return NULL;
}

int PaeHostGetVlanId(int wan_port)
{
	struct local_if *if_entry = NULL;
	int n;

	if (!PaeIpsecInitialized) {
		return 0;
	}

	for (n = 0; n < LOCAL_IF_COUNT; n++) {
		if_entry = &local_if_info[n];
		if (if_entry->vlan_id && !if_entry->wan & !wan_port) {
			return if_entry->vlan_id;
		}
	}
	for (n = 0; n < LOCAL_IF_COUNT; n++) {
		if_entry = &local_if_info[n];
		if (if_entry->vlan_id && if_entry->wan & wan_port) {
			return if_entry->vlan_id;
		}
	}
	return 0;
}

struct local_if *PaeHostMatchLocalAddr(uint8_t *addr, unsigned ip_version)
{
	struct local_if *if_entry = NULL;
	int ip_addr_len = SIZE_IPV4_ADDR;
	int n;

	if (!PaeIpsecInitialized) {
		return NULL;
	}

	if (ip_version == 6) {
		ip_addr_len = SIZE_IPV6_ADDR;
	}
	for (n = 0; n < LOCAL_IF_COUNT; n++) {
		if_entry = &local_if_info[n];
		if (!memcmp(if_entry->ip_addr, addr, ip_addr_len)) {
			return if_entry;
		}
	}
	return NULL;
}

static int update_local_if_info(struct net_device *dev, unsigned long event)
{
	struct vlan_dev_priv *vlan;
	struct local_if *if_entry = NULL;
	struct in_device *in_dev;
	int n;
	//int i;
	__be32 addr = 0;

	if (!PaeIpsecInitialized) {
		return 0;
	}

	// determine the ip version of the interface
	for (n = 0; n < LOCAL_IF_COUNT; n++) {
		if_entry = &local_if_info[n];
		if (!if_entry->dev) {
			if_entry->dev = dev;
			if_entry->ip_ver = 4;
			sprintf(if_entry->if_name, "%s", "eth2.");
			break;
		}
		if (if_entry->dev == dev) {
			break;
		}
	}
	if (n < LOCAL_IF_COUNT) {
		n--;
		netif_addr_lock_bh(dev);
		//printk("if entry %d update ", n);
		switch(event) {
			case NETDEV_POST_INIT:
				/* Occurs when vlan id changes */
				vlan = vlan_dev_priv(dev);
				if_entry->vlan_id = vlan->vlan_id;
				sprintf(&if_entry->if_name[5], "%u", vlan->vlan_id);
				//printk("vlan id %d\n", vlan->vlan_id);
				break;
			case NETDEV_CHANGEADDR:
			case NETDEV_UP:
				/* Change MAC address : copy IP addr and MAC addr */
				if (memcmp(if_entry->mac_addr, dev->dev_addr, dev->addr_len)) {
					memcpy(if_entry->mac_addr, dev->dev_addr, dev->addr_len);
					//printk("updating mac addr ");
					//for (i = 0; i < dev->addr_len; i++) printk("%02x:", dev->dev_addr[i]);
				}
				rcu_read_lock();
				in_dev = __in_dev_get_rcu(dev);
				if (!in_dev) {
					goto no_in_dev;
				}

				for_primary_ifa(in_dev) {
					addr = ifa->ifa_local;
					if (addr) {
						memcpy(if_entry->ip_addr, &addr, SIZE_IPV4_ADDR);
						//printk("  ip addr %x\n", (unsigned)addr);
						break;
					}
				} endfor_ifa(in_dev);
no_in_dev:
				rcu_read_unlock();
				break;
		}
		netif_addr_unlock_bh(dev);
	}
	return 0;
}


/* arp chain event handler function */
int notify_event_handler(struct notifier_block *self, unsigned long event , void *ptr)
{
	struct net_device *dev = (struct net_device *)ptr;
	switch(event) {
		case NETDEV_CHANGEADDR:
			update_local_if_info(dev, event);
			break;
		case NETDEV_POST_INIT:
			update_local_if_info(dev, event);
			break;
		case NETDEV_UP:
			update_local_if_info(dev, event);
			break;
		case NETDEV_DOWN:
		case NETDEV_REBOOT:
		case NETDEV_CHANGE:
		case NETDEV_REGISTER:
		case NETDEV_UNREGISTER:
		case NETDEV_CHANGEMTU:
		case NETDEV_UNREGISTER_BATCH:
		case NETDEV_GOING_DOWN:
		case NETDEV_CHANGENAME:
		case NETDEV_FEAT_CHANGE:
		case NETDEV_BONDING_FAILOVER:
		case NETDEV_PRE_UP:
		case NETDEV_PRE_TYPE_CHANGE:
		case NETDEV_POST_TYPE_CHANGE:
		case NETDEV_RELEASE:
		case NETDEV_NOTIFY_PEERS:
		case NETDEV_JOIN:
			break;
		default:
			//printk("unkown net dev event\n");  
			break;
	}
	return 0;
}

/* Declare and populate the structure */
struct notifier_block notify_block_ipsec = {
	notify_event_handler, 
	NULL, 
	1
};

int pae_netdev_notifier_register(void)
{
	/* Registering with Net Device Chain */
	if ( register_netdevice_notifier( &notify_block_ipsec ) < 0 ) {
		printk("Net Device Chain registration failed!");
	}

	return 0;
}

void pae_netdev_notifier_unregister(void)
{
	/* Unregistering with Net Device Chain */
	if ( unregister_netdevice_notifier( &notify_block_ipsec ) < 0 ){
		printk("Net Device Chain unregistration failed!");
	}
}

int PaeIpsecInit (void)
{
	PaeIpsecInitialized = 1;
	memset(local_if_info, 0, LOCAL_IF_COUNT*sizeof(struct local_if));
	return pae_netdev_notifier_register();
}

int PaeIpsecShutdown (void)
{
	PaeIpsecInitialized = 0;
	memset(local_if_info, 0, LOCAL_IF_COUNT*sizeof(struct local_if));
	pae_netdev_notifier_unregister();
	return 0;
}

/*------------------- ARP notifier ---------------------*/
static int arp_notify_event(struct notifier_block *this, unsigned long event, void *ptr) 
{
    printk(KERN_ERR "arp notify message %s\n", (char *)ptr);
    return 0;
}

struct notifier_block test_notifier = {
    arp_notify_event,   // handler
    NULL,               // parameter is inserted later
    0                   // priority
};

void pae_ipsec_register_arp_callback(uint8_t *ip_addr, char *if_name, char *action_id_string)
{
    if (pae_add_arp_watch_notifier(ip_addr, if_name, &test_notifier, action_id_string))
    {
        printk(KERN_ERR "Interface %s pair %08x is not in the arp watch table\n", if_name, *(unsigned*)ip_addr);
    } else {
		printk("arp notify scheduled for addr %08x\n", *(unsigned*)ip_addr);
	}
}

int pae_ipsec_add_arp_watch(struct local_if *local_if, int action_id)
{
	char action_id_string[5];
	int status;

	sprintf(action_id_string, "%d", action_id);
	status = pae_add_arp_watch(local_if->ip_addr, local_if->mac_addr, local_if->if_name);
	if (!status) {
		pae_ipsec_register_arp_callback(local_if->ip_addr, local_if->if_name, action_id_string);
	}
	return status;
}

EXPORT_SYMBOL(PaeIpsecInit);
