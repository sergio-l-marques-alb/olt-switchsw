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

#ifndef PAE_NOTIFY_H_
#define PAE_NOTIFY_H_

#define LOCAL_IF_COUNT		6

#define LAN_VLAN_ID			0
#define WAN_VLAN_ID			1

struct local_if {
	struct net_device *dev;
	uint8_t ip_ver;
	uint8_t wan; 
	uint16_t vlan_id;
	uint8_t mac_addr[6];
	uint8_t ip_addr[16];
	char if_name[10];
};

int PaeHostGetVlanId(int wan_port);
struct local_if * PaeHostMatchLocalAddr(uint8_t *addr, unsigned ip_version);
struct local_if *PaeHostGetLanInterface(void);
int pae_ipsec_add_arp_watch(struct local_if *lif, int action_id);
void pae_ipsec_register_arp_callback(uint8_t *ip_addr, char *if_name, char *action_id_string);
#endif /* PAE_NOTIFY_H_ */
