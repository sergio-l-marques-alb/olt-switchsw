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
/******************************************************************************/
/* This is a rudimentary software implementation of header translation as     */
/*   per NAT64.  It currently only does one direction (v6->v4, outbound)      */
/*   and only UDP (not TCP or ICMP).  It is not meant to be a useful NAT64    */
/*   implementation, but is useful to show how a PAE rule/action can be used  */
/*   to accelerate a rewrite, and to compare the CPU utilization of SW vs PAE */
/******************************************************************************/

#include <linux/err.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cpu.h>
#include <net/ip6_route.h>
#include <net/flow.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_extend.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_rule.h>
#include <net/netfilter/nf_nat_protocol.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_nat_helper.h>

struct pae_nat64_tuple {
    __be32 ipv4_local_addr;
    __be32 ipv4_remote_addr;
    __be16 ipv4_local_port;
    __be16 ipv4_remote_port;

    struct in6_addr ipv6_remote_addr;
    struct in6_addr ipv6_local_addr;
    __be16 ipv6_remote_port;
    __be16 ipv6_local_port;
    
    unsigned int proto;
    
    u8 ipv6_remote_mac[6];
};

static struct pae_nat64_tuple pae_nat64_single;
static int single_setup = 0;

static struct socket *ipv4_udp_sock = 0;
static struct sockaddr_in ipv4_udp_sin;
static struct msghdr ipv4_udp_msg;
static struct iovec ipv4_udp_iov;

int pae_ipv4_match(const struct pae_nat64_tuple *a, const struct pae_nat64_tuple *b) {
    return (a->ipv4_local_addr == b->ipv4_local_addr &&
            a->ipv4_remote_addr == b->ipv4_remote_addr &&
            a->ipv4_local_port == b->ipv4_local_port &&
            a->ipv4_remote_port == b->ipv4_remote_port &&
            a->proto == b->proto);
}

int pae_ipv6_match(const struct pae_nat64_tuple *a, const struct pae_nat64_tuple *b) {
    return (ipv6_addr_equal(&a->ipv6_remote_addr, &b->ipv6_remote_addr) &&
            ipv6_addr_equal(&a->ipv6_local_addr, &b->ipv6_local_addr) &&
            a->ipv6_remote_port == b->ipv6_remote_port &&
            a->ipv6_local_port == b->ipv6_local_port &&
            a->proto == b->proto);
}


void pae_netfilter_single_setup(void)
{
    int ret;

    /* Configure one (static) mapping to be used for demonstration purposes */
    pae_nat64_single.ipv4_local_addr = htonl((192<<24) | (168 << 16) | (2 << 8) | (1));
    pae_nat64_single.ipv4_remote_addr = htonl((192<<24) | (168 << 16) | (2 << 8) | (33));
    /* local port set below */
    pae_nat64_single.ipv4_remote_port = htons(319);

    pae_nat64_single.ipv6_remote_addr.s6_addr32[0] = htonl(0xfe800000);
    pae_nat64_single.ipv6_remote_addr.s6_addr32[1] = htonl(0x00000000);
    pae_nat64_single.ipv6_remote_addr.s6_addr32[2] = htonl(0x7a2bcbff);
    pae_nat64_single.ipv6_remote_addr.s6_addr32[3] = htonl(0xfe916be7);

    pae_nat64_single.ipv6_local_addr.s6_addr32[0] = htonl(0xfe800000);
    pae_nat64_single.ipv6_local_addr.s6_addr32[1] = htonl(0x00000000);
    pae_nat64_single.ipv6_local_addr.s6_addr32[2] = htonl(0x020000ff);
    pae_nat64_single.ipv6_local_addr.s6_addr32[3] = htonl(0xfe000005);

    pae_nat64_single.ipv6_remote_port = htons(319);
    pae_nat64_single.ipv6_local_port = htons(319);

    pae_nat64_single.proto = IPPROTO_UDP;

    /* Also set up an IPv4 UDP socket for sending (with routing) on WAN side */
    ret = sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &ipv4_udp_sock);
    if (ret < 0) {
        pr_err("Can't create socket for NAT64\n");
        return;
    }

    ipv4_udp_sin.sin_family = AF_INET;
    ipv4_udp_sin.sin_port = pae_nat64_single.ipv4_remote_port;
    ipv4_udp_sin.sin_addr.s_addr = pae_nat64_single.ipv4_remote_addr;

    ret = ipv4_udp_sock->ops->connect(ipv4_udp_sock, (struct sockaddr *)&ipv4_udp_sin, sizeof(struct sockaddr), 0);
    if (ret < 0) {
        pr_err("Can't connect socket for NAT64\n");
        /* Should delete ipv4_udp_sock... */
        ipv4_udp_sock = 0;
        return;
    }

    /* Now that we have our socket, grab the local port number for our mapping */
    pae_nat64_single.ipv4_local_port = inet_sk(ipv4_udp_sock->sk)->inet_sport;

    /* Preparing message header */
    ipv4_udp_msg.msg_flags = 0;
    ipv4_udp_msg.msg_name = &ipv4_udp_sin;
    ipv4_udp_msg.msg_namelen  = sizeof(struct sockaddr_in);
    ipv4_udp_msg.msg_control = NULL;
    ipv4_udp_msg.msg_controllen = 0;
    ipv4_udp_msg.msg_iov = &ipv4_udp_iov;
    ipv4_udp_msg.msg_control = NULL;
}


static unsigned int
nf_paesw_ipv4_in(unsigned int hooknum,
	  struct sk_buff *skb,
	  const struct net_device *in,
	  const struct net_device *out,
	  int (*okfn)(struct sk_buff *))
{
	unsigned int ret = NF_ACCEPT;
    unsigned int iphdroff = skb_network_offset(skb);
	const struct iphdr *iph = (struct iphdr *)(skb->data + iphdroff);
    const u8 *l4hdr = skb->data + iphdroff + iph->ihl*4;

    struct pae_nat64_tuple pkt_tuple = {
        .proto = iph->protocol,
        .ipv4_local_addr = iph->daddr,
        .ipv4_remote_addr = iph->saddr
    };

    if (!single_setup) {
        pae_netfilter_single_setup();
        single_setup = 1;
    }

    switch (iph->protocol) {
    case IPPROTO_UDP:
        {
            const struct udphdr *udphdr = (struct udphdr *)l4hdr;
            pkt_tuple.ipv4_remote_port = udphdr->source;
            pkt_tuple.ipv4_local_port = udphdr->dest;
        }
        break;
    case IPPROTO_TCP:
        {
            const struct tcphdr *tcphdr = (struct tcphdr *)l4hdr;
            pkt_tuple.ipv4_remote_port = tcphdr->source;
            pkt_tuple.ipv4_local_port = tcphdr->dest;
        }
        break;
    default:
        /* NAT64 / 4RD should deal with ICMP as well, but this is a stub, so just let them through */
        return NF_ACCEPT;
    }

    if (pae_ipv4_match(&pkt_tuple, &pae_nat64_single)) {
        return NF_DROP;
    }

	return ret;
}

static unsigned int
nf_paesw_ipv6_in(unsigned int hooknum,
	  struct sk_buff *skb,
	  const struct net_device *in,
	  const struct net_device *out,
	  int (*okfn)(struct sk_buff *))
{
	unsigned int ret = NF_ACCEPT;

    const struct ipv6hdr *iph = ipv6_hdr(skb);
    int proto;
    int hlen = sizeof(struct ipv6hdr);

	if (iph == NULL) {
        printk("No addr info\n");
		return ret;
    }
    proto = iph->nexthdr;
    
    if (proto == IPPROTO_FRAGMENT) {
        proto = *(u8*)(iph + 1);
        hlen += 8;
    }

    switch (proto) {
    case IPPROTO_UDP:
        {
            struct udphdr * udphdr = (struct udphdr *)((u8 *)iph + hlen);
            struct pae_nat64_tuple pkt_tuple;
            pkt_tuple.proto = proto;
            memcpy(&pkt_tuple.ipv6_remote_addr, &iph->saddr, sizeof(struct in6_addr));
            memcpy(&pkt_tuple.ipv6_local_addr, &iph->daddr, sizeof(struct in6_addr));
            pkt_tuple.ipv6_remote_port = udphdr->source;
            pkt_tuple.ipv6_local_port = udphdr->dest;

            if (pae_ipv6_match(&pkt_tuple, &pae_nat64_single)) {
                int err;
                int len = ntohs(udphdr->len);
                ipv4_udp_iov.iov_base = (u8 *)iph + hlen + sizeof(struct udphdr);
                ipv4_udp_iov.iov_len = len;
                ipv4_udp_msg.msg_iovlen = len;
                err = sock_sendmsg(ipv4_udp_sock, &ipv4_udp_msg, len);
                if (err < 0) {
                    pr_err("Failed sending UDP: %d\n", err);
                } else {
                    printk("Sent\n");
                }
                /* struct sk_buff *ipv4_skb = alloc_skb(LL_MAX_HEADER + sizeof(struct iphdr) + skb->len, GFP_ATOMIC); */
                /* if (!ipv4_skb) { */
                /*     return NF_ACCEPT; */
                /* } */

                /* skb_reserve(ipv4_skb, LL_MAX_HEADER + sizeof(struct iphdr)); */


                /* skb_put(ipv4_skb, skb->len); */

                
                /* { */
                /*     struct iphdr *ipv4_hdr = ip_hdr(ipv4_skb); */
                /*     u8 *ip4_l4 = (((u8 *)ipv4_hdr) + sizeof(struct iphdr)); */
                /*     struct udphdr *ip4_udphdr = (struct udphdr *)ip4_l4; */
                /*     ipv4_hdr->version = 4; */
                /*     ipv4_hdr->ihl = 20/sizeof(u32); */
                    
                /*     ipv4_hdr->tos = 0; */
                /*     ipv4_hdr->tot_len = htons(skb->len) + sizeof(struct iphdr); */
                /*     ipv4_hdr->protocol = proto; */
                /*     ipv4_hdr->ttl = iph->hop_limit; */
                /*     ipv4_hdr->saddr = pae_nat64_single.ipv4_local_addr; */
                /*     ipv4_hdr->daddr = pae_nat64_single.ipv4_remote_addr; */
                /*     memcpy(ip4_l4, ((u8 *)iph + hlen), skb->len); */

                /*     ip4_udphdr->source = pae_nat64_single.ipv4_local_port; */
                /*     ip4_udphdr->dest = pae_nat64_single.ipv4_remote_port; */
                /* } */

                /* dev_queue_xmit(ipv4_skb); */

                return NF_DROP;
            } else {
                return NF_ACCEPT;
            }
        }

    case IPPROTO_TCP:
        /* Only dealing with UDP for now */
        return NF_ACCEPT;
    case IPPROTO_ICMPV6:
        /* Only dealing with UDP for now */
        return NF_ACCEPT;
    default:
        return NF_ACCEPT;
    }

    /*
     * printk("IPv6 DA: %08x%08x%08x%08x\n", iph->daddr.s6_addr32[0], iph->daddr.s6_addr32[1], iph->daddr.s6_addr32[2], iph->daddr.s6_addr32[3]);
    */
	return ret;
}

static struct nf_hook_ops nf_paesw_ops[] __read_mostly = {
	/* Before packet filtering, change destination */
	{
		.hook		= nf_paesw_ipv4_in,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV4,
		.hooknum	= NF_INET_PRE_ROUTING,
		.priority	= NF_IP_PRI_NAT_DST,
	},
	{
		.hook		= nf_paesw_ipv6_in,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV6,
		.hooknum	= NF_INET_PRE_ROUTING,
		.priority	= NF_IP_PRI_NAT_DST,
	},
};

void pae_netfilter_setup(void)
{
    int ret;

	ret = nf_register_hooks(nf_paesw_ops, ARRAY_SIZE(nf_paesw_ops));
	if (ret < 0) {
		pr_err("pae_netfilter_setup: can't register hooks.\n");
	}
}
