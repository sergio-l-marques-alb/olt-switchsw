/* 
 * $Id: shr_pkt_pack.c, Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        shr_pkt.c
 * Purpose:     Common pkt pack/unpack routines and pkt utility
 *              functions.
 *
 */
#include <soc/shared/shr_pkt.h>

/*
 * Functions:
 *      shr_<header>_pack
 * Purpose:
 *      The following set of _pack() functions packs in
 *      network byte order a given header.
 * Parameters:
 *      buffer          - (OUT) Buffer where to pack header.
 *      <header>        - (IN) Header to pack.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 */

uint8 *
shr_udp_header_pack(uint8 *buffer, shr_udp_header_t *udp)
{
   SHR_PKT_PACK_U16(buffer, udp->src);
   SHR_PKT_PACK_U16(buffer, udp->dst);
   SHR_PKT_PACK_U16(buffer, udp->length);
   SHR_PKT_PACK_U16(buffer, udp->sum);

    return buffer;
}

uint8 *
shr_ipv4_header_pack(uint8 *buffer, shr_ipv4_header_t *ip)
{
    uint32  tmp;

    tmp = ((ip->version & 0xf) << 28) | ((ip->h_length & 0xf) << 24) |
        ((ip->dscp & 0x3f) << 18) | ((ip->ecn & 0x3)<< 16) | (ip->length);
    SHR_PKT_PACK_U32(buffer, tmp);

    tmp = (ip->id << 16) | ((ip->flags & 0x7) << 13) | (ip->f_offset & 0x1fff);
    SHR_PKT_PACK_U32(buffer, tmp);

    tmp = (ip->ttl << 24) | (ip->protocol << 16) | ip->sum;
    SHR_PKT_PACK_U32(buffer, tmp);

    SHR_PKT_PACK_U32(buffer, ip->src);
    SHR_PKT_PACK_U32(buffer, ip->dst);

    return buffer;
}

uint8 *
shr_ipv6_header_pack(uint8 *buffer, shr_ipv6_header_t *ip)
{
    uint32  tmp;
    int     i;

    tmp = ((ip->version & 0xf) << 28) | (ip->t_class << 20) |
        (ip->f_label & 0xfffff);
    SHR_PKT_PACK_U32(buffer, tmp);

    tmp = (ip->p_length << 16 ) | (ip->next_header << 8) | ip->hop_limit;
    SHR_PKT_PACK_U32(buffer, tmp);

    for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
        SHR_PKT_PACK_U8(buffer, ip->src[i]);
    }

    for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
        SHR_PKT_PACK_U8(buffer, ip->dst[i]);
    }

    return buffer;
}

uint8 *
shr_gre_header_pack(uint8 *buffer, shr_gre_header_t *gre)
{
    uint32  tmp;

    tmp = ((gre->checksum & 0x1) << 31) | ((gre->routing & 0x1) << 30) |
        ((gre->key & 0x1) << 29) | ((gre->sequence_num & 0x1) << 28) | 
        ((gre->strict_src_route & 0x1) << 27) |
        ((gre->recursion_control & 0x7) << 24) | ((gre->flags & 0x1f) << 19) |
        ((gre->version & 0x7) << 16) | (gre->protocol & 0xffff);

    SHR_PKT_PACK_U32(buffer, tmp);

    return buffer;
}

uint8 *
shr_ach_header_pack(uint8 *buffer, shr_ach_header_t *ach)
{
    uint32  tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) | ((ach->version & 0xf) << 24) |
        (ach->reserved << 16) | ach->channel_type;

    SHR_PKT_PACK_U32(buffer, tmp);

    return buffer;
}

uint8 *
shr_mpls_label_pack(uint8 *buffer, shr_mpls_label_t *mpls)
{
    uint32  tmp;

    tmp = ((mpls->label & 0xfffff) << 12) | ((mpls->exp & 0x7) << 9) |
        ((mpls->s & 0x1) << 8) | mpls->ttl;
    SHR_PKT_PACK_U32(buffer, tmp);

    return buffer;
}

uint8 *
shr_l2_header_pack(uint8 *buffer, shr_l2_header_t *l2)
{
    uint32  tmp;
    int     i;

    for (i = 0; i < SHR_MAC_ADDR_LENGTH; i++) {
        SHR_PKT_PACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < SHR_MAC_ADDR_LENGTH; i++) {
        SHR_PKT_PACK_U8(buffer, l2->src_mac[i]);
    }

    /*
     * VLAN Tag
     *
     * A TPID value of 0 indicates this is an untagged frame.
     */
    if (l2->outer_vlan_tag.tpid != 0) {
        /* Add VLAN tag */
        tmp = (l2->outer_vlan_tag.tpid << 16) |
            ((l2->outer_vlan_tag.tci.prio & 0x7) << 13) |
            ((l2->outer_vlan_tag.tci.cfi & 0x1) << 12) |
            (l2->outer_vlan_tag.tci.vid & 0xfff);
        SHR_PKT_PACK_U32(buffer, tmp);
    }

    /* Packet inner vlan tag */

    if (l2->inner_vlan_tag.tpid != 0) {
        /* Add VLAN tag */
        tmp = (l2->inner_vlan_tag.tpid << 16) |
            ((l2->inner_vlan_tag.tci.prio & 0x7) << 13) |
            ((l2->inner_vlan_tag.tci.cfi & 0x1) << 12) |
            (l2->inner_vlan_tag.tci.vid & 0xfff);
        SHR_PKT_PACK_U32(buffer, tmp);
    }

    SHR_PKT_PACK_U16(buffer, l2->etype);

    return buffer;
}

/*
 * Function:
 *      shr_udp_initial_checksum_get
 * Purpose:
 *      Get the initial checksum
 *
 *      Return the partial checksum as uint32 value. This routine simply adds up
 *      the data as 16 bit words. The final 16 bit checksum has to be calculated
 *      later.
 * Parameters:
 *      length             - (IN) Length of the data buffer
 *      data               - (IN) Data buffer
 * Returns:
 *      Initial checksum
 */
uint32
shr_initial_chksum_get(uint16 length, uint8 *data)
{
    uint32  chksum = 0;
    uint16  w16;
    int     i = 0;

    while (length > 1) {
        w16 = (((uint16)data[i]) << 8) + data[i+1];
        chksum += w16;
        i+=2;
        length -= 2;
    }
    if (length) {
        w16 = (((uint16)data[i]) << 8) + 0;
        chksum += w16;
    }

    return (chksum);
}

/*
 * Function:
 *      shr_udp_initial_checksum_get
 * Purpose:
 *      Get the UDP initial checksum (excludes checksum for the data).
 *
 *      The checksum includes the IP pseudo-header and UDP header.
 *      It does not include the checksum for the data.
 *      The data checksum will be added to UDP initial checksum
 *      each time a packet is sent, since data payload may vary.
 * Parameters:
 *      v6              - (IN) Set if IP is IPv6.
 *      ipv4            - (IN) IPv4 header.
 *      ipv6            - (IN) IPv6 header.
 *      udp             - (IN/OUT) UDP header checksum to update.
 * Returns:
 *      Initial checksum
 */
uint32
shr_udp_initial_checksum_get(int v6,
                             shr_ipv4_header_t *ipv4,
                             shr_ipv6_header_t *ipv6,
                             shr_udp_header_t  *udp)
{
    uint8  buffer[SHR_UDP_HEADER_LENGTH + (BCM_IP6_ADDRLEN*2) + 8];
    uint8  *cur_ptr = buffer;
    int    i, length;

    /* Build IP Pseudo-Header */
    if (v6) {
        /* IPv6 Pseudo-Header
         *     Source address
         *     Destination address
         *     UDP length  (32-bit)
         *     Next Header (lower 8 bits of 32-bit)
         */
        for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
            SHR_PKT_PACK_U8(cur_ptr, ipv6->src[i]);
        }
        for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
            SHR_PKT_PACK_U8(cur_ptr, ipv6->dst[i]);
        }
        SHR_PKT_PACK_U16(cur_ptr, udp->length);
        SHR_PKT_PACK_U8(cur_ptr, ipv6->next_header);
    } else {
        /* IPv4 Pseudo-Header
         *     Source address
         *     Destination address
         *     Protocol    (8 bits)
         *     UDP length  (16-bit)
         */
        SHR_PKT_PACK_U32(cur_ptr, ipv4->src);
        SHR_PKT_PACK_U32(cur_ptr, ipv4->dst);
        /*
         * Added 0 before protcol value to compute checksum
         * accurately
         */
        SHR_PKT_PACK_U8(cur_ptr, 0);
        SHR_PKT_PACK_U8(cur_ptr, ipv4->protocol);
        SHR_PKT_PACK_U16(cur_ptr, udp->length);
    }

    /* Add UDP header */
    cur_ptr = shr_udp_header_pack(cur_ptr, udp);

    /* Calculate initial UDP checksum */
    length = cur_ptr - buffer;
    return shr_initial_chksum_get(length, buffer);
}
