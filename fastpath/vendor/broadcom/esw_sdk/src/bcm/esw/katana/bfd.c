/*
 * $Id: bfd.c 1.37.2.116 Broadcom SDK $
 *
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
 * File:       bfd.c
 * Purpose:    Bidirectional Forwarding Detection APIs.
 *
 * Notes:      BFD functions will return BCM_E_UAVAIL unless these conditions
 *             are true:
 *               - BCM_CMICM_SUPPORT
 *               - soc_feature_cmicm
 *
 */

#include <soc/defs.h>

#if defined(BCM_KATANA_SUPPORT) && defined(INCLUDE_BFD)

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/higig.h>
#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/l3x.h>
#include <soc/katana.h>
#include <shared/pack.h>
#include <bcm/bfd.h>

#if defined(BCM_CMICM_SUPPORT)
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/bfd.h>
#include <soc/shared/bfd_pkt.h>
#include <soc/shared/bfd_msg.h>
#include <soc/shared/bfd_pack.h>
#endif /* BCM_CMICM_SUPPORT */

#include <bcm_int/common/rx.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/bfd.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/mpls.h>

#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#undef _BFD_DEBUG_DUMP

#ifdef BCM_CMICM_SUPPORT

#define BFD_SDK_VERSION         0x01000001
#define BFD_UC_MIN_VERSION      0x01000200

#define BFD_COSQ_INVALID        0xffff


/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(_arg)  \
    if ((_arg) == NULL) {       \
        return BCM_E_PARAM;     \
    }

/* Checks that required feature(s) is available */
#define FEATURE_CHECK(_unit)                      \
    if (!soc_feature(_unit, soc_feature_cmicm)) { \
        return BCM_E_UNAVAIL;                     \
    } 

/* Checks that BFD module has been initialized */
#define INIT_CHECK(_unit)                   \
    if (!_kt_bfd_info[_unit].initialized) { \
        return BCM_E_INIT;                  \
    }

/* Checks that endpoint index is within valid range */
#define ENDPOINT_INDEX_CHECK(_unit, _index)                               \
    if ((_index) < 0 || (_index) >= _kt_bfd_info[_unit].endpoint_count) { \
        return BCM_E_PARAM;                                               \
    }

/* Checks that SHA1 Authentication index is valid */
#define AUTH_SHA1_INDEX_CHECK(_unit, _index)                                  \
    if ((_index) < 0 || (_index) >= _kt_bfd_info[_unit].num_auth_sha1_keys) { \
        return BCM_E_PARAM;                                                   \
    }

/* Checks that Simple Password Authentication index is valid */
#define AUTH_SP_INDEX_CHECK(_unit, _index)                                  \
    if ((_index) < 0 || (_index) >= _kt_bfd_info[_unit].num_auth_sp_keys) { \
        return BCM_E_PARAM;                                                 \
    }

#define BFD_INFO(_unit)                         \
    (&_kt_bfd_info[_unit])
#define BFD_ENDPOINT_CONFIG(_unit, _index)    \
    (&_kt_bfd_info[_unit].endpoints[_index])

/*
 * Maximum number of BFD endpoints
 * Endpoint IDs valid range is  [0..2046]
 * Entry 0x7FF (2047) is reserved for unknown remote discriminator
 */
#define _BFD_MAX_NUM_ENDPOINTS             2047

/* Timeout for Host <--> uC message */
#define _BFD_UC_MSG_TIMEOUT_USECS          20000000


/*
 * BFD Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a BFD packet.
 */
#define _BFD_ENCAP_PKT_MPLS                    (1 << 0) 
#define _BFD_ENCAP_PKT_MPLS_ROUTER_ALERT       (1 << 1) 
#define _BFD_ENCAP_PKT_PW                      (1 << 2) 
#define _BFD_ENCAP_PKT_GAL                     (1 << 3) 
#define _BFD_ENCAP_PKT_G_ACH                   (1 << 4) 
#define _BFD_ENCAP_PKT_GRE                     (1 << 5) 
#define _BFD_ENCAP_PKT_INNER_IP                (1 << 6)
#define _BFD_ENCAP_PKT_IP                      (1 << 7) 
#define _BFD_ENCAP_PKT_UDP                     (1 << 8) 
#define _BFD_ENCAP_PKT_BFD                     (1 << 9)
#define _BFD_ENCAP_PKT_HG                      (1 << 10)
#define _BFD_ENCAP_PKT_HG2                     (1 << 11)

#define _BFD_ENCAP_PKT_UDP__MULTI_HOP          (1 << 16) 
#define _BFD_ENCAP_PKT_INNER_IP__V6            (1 << 17)
#define _BFD_ENCAP_PKT_IP__V6                  (1 << 18) 
#define _BFD_ENCAP_PKT_G_ACH__IP               (1 << 19)
#define _BFD_ENCAP_PKT_G_ACH__CC               (1 << 20)
#define _BFD_ENCAP_PKT_G_ACH__CC_CV            (1 << 21)




/*
 * BFD Encapsulation Definitions
 *
 * Defines for building the BFD packet encapsulation
 */

/*
 * Macros to pack uint8, uint16, uint32 in Network byte order
 */
#define _BFD_ENCAP_PACK_U8(_buf, _var)   SHR_BFD_ENCAP_PACK_U8(_buf, _var)
#define _BFD_ENCAP_PACK_U16(_buf, _var)  SHR_BFD_ENCAP_PACK_U16(_buf, _var)
#define _BFD_ENCAP_PACK_U32(_buf, _var)  SHR_BFD_ENCAP_PACK_U32(_buf, _var)

#define _BFD_MAC_ADDR_LENGTH    (sizeof(bcm_mac_t))

#define _BFD_MPLS_MAX_LABELS    3   /* Max MPLS labels in Katana */


/* UDP Header */
typedef struct _udp_header_s {  /* <num bits>: ... */
    uint16  src;         /* 16: Source Port Number */
    uint16  dst;         /* 16: Destination Port Number */
    uint16  length;      /* 16: Length */
    uint16  sum;         /* 16: Checksum */
} _udp_header_t;

/* IPv4 Header */
typedef struct _ipv4_header_s {
    uint8   version;     /*  4: Version */
    uint8   h_length;    /*  4: Header length */
    uint8   dscp;        /*  6: Differentiated Services Code Point */
    uint8   ecn;         /*  2: Explicit Congestion Notification */
    uint16  length;      /* 16: Total Length bytes (header + data) */
    uint16  id;          /* 16: Identification */
    uint8   flags;       /*  3: Flags */
    uint16  f_offset;    /* 13: Fragment Offset */
    uint8   ttl;         /*  8: Time to Live */
    uint8   protocol;    /*  8: Protocol */
    uint16  sum;         /* 16: Header Checksum */
    uint32  src;         /* 32: Source IP Address */
    uint32  dst;         /* 32: Destination IP Address */
} _ipv4_header_t;

/* IPv6 Header */
typedef struct _ipv6_header_s {
    uint8      version;        /*   4: Version */
    uint8      t_class;        /*   8: Traffic Class (6:dscp, 2:ecn) */
    uint32     f_label;        /*  20: Flow Label */
    uint16     p_length;       /*  16: Payload Length */
    uint8      next_header;    /*   8: Next Header */
    uint8      hop_limit;      /*   8: Hop Limit */
    bcm_ip6_t  src;            /* 128: Source IP Address */
    bcm_ip6_t  dst;            /* 128: Destination IP Address */
} _ipv6_header_t;

/* GRE - Generic Routing Encapsulation */
typedef struct _gre_header_s {
    uint8   checksum;           /*  1: Checksum present */
    uint8   routing;            /*  1: Routing present */
    uint8   key;                /*  1: Key present */
    uint8   sequence_num;       /*  1: Sequence number present */
    uint8   strict_src_route;   /*  1: Strict Source Routing */
    uint8   recursion_control;  /*  3: Recursion Control */
    uint8   flags;              /*  5: Flags */
    uint8   version;            /*  3: Version */
    uint16  protocol;           /* 16: Protocol Type */
} _gre_header_t;

/* ACH - Associated Channel Header */
typedef struct _ach_header_s {
    uint8   f_nibble;        /*  4: First nibble, must be 1 */
    uint8   version;         /*  4: Version */
    uint8   reserved;        /*  8: Reserved */
    uint16  channel_type;    /* 16: Channel Type */
} _ach_header_t;

/* MPLS - Multiprotocol Label Switching Label */
typedef struct _mpls_label_s {
    uint32  label;    /* 20: Label */
    uint8   exp;      /*  3: Experimental, Traffic Class, ECN */
    uint8   s;        /*  1: Bottom of Stack */
    uint8   ttl;      /*  8: Time to Live */
} _mpls_label_t;

/* VLAN Tag - 802.1Q */
typedef struct _vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} _vlan_tag_t;

/* L2 Header */
typedef struct _l2_header_t {
    bcm_mac_t    dst_mac;     /* 48: Destination MAC */
    bcm_mac_t    src_mac;     /* 48: Source MAC */
    _vlan_tag_t  vlan_tag;    /* VLAN Tag */
    uint16       etype;       /* 16: Ether Type */
} _l2_header_t;


/*
 * UDP Definitions for BFD
 */
#define _BFD_UDP_SRCPORT_MIN                49152
#define _BFD_UDP_SRCPORT_MAX                65535
#define ENCAP_UDP_SRCPORT_VALIDATE(_a)                                    \
    if (((_a) < _BFD_UDP_SRCPORT_MIN) || ((_a) > _BFD_UDP_SRCPORT_MAX)) { \
        return BCM_E_PARAM;                                             \
    }

/*
 * IPV4
 *
 * Loopback    127.0.0.0/8         127.0.0.0   - 127.255.255.255
 *
 * Reserved
 *   Class A   10.0.0.0/8          10.0.0.0    - 10.255.255.255
 *   Class B   172.16.0.0/12       172.16.0.0  - 172.31.255.255
 *   Class C   192.168.0.0/16      192.168.0.0 - 192.168.255.255  
 *   Class E   240.0.0.0
 */
#define IPV4_LOOPBACK(_a)    (127 == (((_a) >> 24) & 0xff))

#define IPV4_RESERVED_CLASS_A(_a)  ((((_a) >> 24) & 0xff) == 10)
#define IPV4_RESERVED_CLASS_B(_a)  (((((_a) >> 24) & 0xff) == 172) &&   \
                                    ((((_a) >> 16) & 0xf0) == 16))
#define IPV4_RESERVED_CLASS_C(_a)  (((((_a) >> 24) & 0xff) == 192) &&   \
                                    ((((_a) >> 16) & 0xff) == 168))
#define IPV4_RESERVED_CLASS_E(_a)  ((((_a) >> 24) & 0xff) == 240)

#define IPV4_RESERVED(_a)                                               \
    (IPV4_RESERVED_CLASS_A(_a) || IPV4_RESERVED_CLASS_B(_a) ||          \
     IPV4_RESERVED_CLASS_C(_a) || IPV4_RESERVED_CLASS_E(_a))

#define ENCAP_IPV4_LOOPBACK_VALIDATE(_a)            \
    if (!IPV4_LOOPBACK(_a)) { return BCM_E_PARAM; }

#define ENCAP_IPV4_ROUTABLE_VALIDATE(_a)            \
    if (IPV4_RESERVED(_a)) { return BCM_E_PARAM; } 

/*
 * IPV6
 * Loopback    0:0:0:0:0:FFFF:127/104
 *
 * Reserved
 *    fc00::/7
 */
#define IPV6_LOOPBACK(_a)                                                    \
    (((_a)[0] == 0) && ((_a)[1] == 0) && ((_a)[2] == 0) && ((_a)[3] == 0) && \
     ((_a)[4] == 0) && ((_a)[5] == 0) && ((_a)[6] == 0) && ((_a)[7] == 0) && \
     ((_a)[8] == 0) && ((_a)[9] == 0) &&                                     \
     ((_a)[10] == 0xff) && ((_a)[11] == 0xff) && ((_a)[12] == 127))
     
#define IPV6_RESERVED(_a)    (((_a)[0] & 0xfe) == 0xfc)

#define ENCAP_IPV6_LOOPBACK_VALIDATE(_a)            \
    if (!IPV6_LOOPBACK(_a)) { return BCM_E_PARAM; }

#define ENCAP_IPV6_ROUTABLE_VALIDATE(_a)            \
    if (IPV6_RESERVED(_a)) { return BCM_E_PARAM; } 



/* BFD Event Handler */
typedef struct _event_handler_s {
    struct _event_handler_s *next;
    bcm_bfd_event_types_t event_types;
    bcm_bfd_event_cb cb;
    void *user_data;
} _event_handler_t;

/* BFD Endpoint */
typedef struct _endpoint_config_s {
    int endpoint_index;
    bcm_module_t  modid;    /* Destination module ID */
    bcm_port_t port;        /* Destination port */
    bcm_port_t tx_port;     /* Local physical port to TX BFD packet */
    int encap_type;         /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    int encap_length;       /* BFD Encapsulation length */
    int lkey_etype;         /* Lookup key Ether Type */
    int lkey_offset;        /* Lookup key offset */
    int lkey_length;        /* Lookup key length */
    bcm_bfd_endpoint_info_t info;
} _endpoint_config_t;

/* BFD Information */
typedef struct _bfd_info_s {
    int unit;                   /* Unit number */
    int initialized;            /* If set, BFD has been initialized */
    int endpoint_count;         /* Max number of BFD endpoints */
    int encap_size;             /* Total encapsulation table size */
    int num_auth_sha1_keys;     /* Max number of sha1 auth keys */
    int num_auth_sp_keys;       /* Max number of simple pwd auth keys */
    bcm_cos_queue_t cpu_cosq;   /* CPU cos queue for RX BFD packets */
    int cosq_spath_index;       /* CPU cos queue map index for good packets */
    int cosq_ach_err_index;     /* CPU cos queue map index for ACH error */
    int rx_channel;             /* Local RX DMA channel for BFD packets */
    int uc_num;                 /* uController number running BFD appl */
    int dma_buffer_len;         /* DMA max buffer size */
    uint8* dma_buffer;          /* DMA buffer */
    SHR_BITDCL *endpoints_in_use;   /* Indicates used endpoint indexes */
    _endpoint_config_t *endpoints;  /* Array to Endpoints */
    bcm_bfd_auth_sha1_t *auth_sha1; /* Array of sha1 auth keys */
    bcm_bfd_auth_simple_password_t *auth_sp; /* Array simple pwd auth keys */
    sal_thread_t event_thread_id;            /* Event handler thread id */
    int event_thread_kill;                   /* Signal Event thread exit */
    _event_handler_t *event_handler_list;    /* List of event handlers */
    uint32 event_mask;                     /* Mask of all events */
} _bfd_info_t;

static _bfd_info_t _kt_bfd_info[BCM_MAX_NUM_UNITS];


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_kt_bfd_warm_boot
 * Purpose:
 *      Recover local data during Warm Boot.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_kt_bfd_warm_boot(int unit)
{

#if 0
    /* Do this get for each possible session for WARMBOOT */
    get_p = &((bfd_msg_ctrl_t*)bfd_info->dmabuf)->u.get;
    sal_memset(get_p, 0, sizeof(bfd_msg_ctrl_sess_get_t));
    soc_cm_sflush(unit, get_p, sizeof(bfd_msg_ctrl_sess_get_t));

    sal_memset(&uc_msg, 0, sizeof(uc_msg));
    uc_msg.s.mclass = MOS_MSG_CLASS_BFD;
    uc_msg.s.subclass = MOS_MSG_SUBCLASS_BFD_SESS_GET;
    uc_msg.s.len = bcm_htons(endpoint_config->endpoint_index);
    uc_msg.s.data = bcm_htonl(soc_cm_l2p(unit, get_p));

    sal_memset(&uc_reply, 0, sizeof(uc_reply));

    result = soc_cmic_uc_msg_send_receive(unit, bfd_info->uc_num,
                                          &uc_msg, &uc_reply,
                                          _BFD_UC_MSG_TIMEOUT_USECS);
    if ((SOC_E_NONE == result) &&
        (uc_reply.s.subclass == MOS_MSG_SUBCLASS_BFD_SESS_GET_REPLY) &&
        (uc_reply.s.mclass == MOS_MSG_CLASS_BFD) ) {
        
    } else {
        result = BCM_E_INTERNAL;
    }

    /* recover each entry of SHA1 authentication table for RAM cache */
    /* recover each entry of simple password authenticaton table for RAM cache
     */
#endif
    return BCM_E_NONE;
}

int
_bcm_kt_bfd_sync(int unit)
{

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *      _bcm_kt_bfd_alloc_clear
 * Purpose:
 *      Allocate memory block and set memory content to zeros.
 * Parameters:
 *      size        - (IN) Size of memory to allocate.
 *      description - (IN) Description of memory block.
 * Returns:
 *      Pointer to memory block.
 */
STATIC void *
_bcm_kt_bfd_alloc_clear(unsigned int size, char *description)
{
    void *block = NULL;

    if (size) {
        block = sal_alloc(size, description);
        if (block != NULL) {
            sal_memset(block, 0, size);
        }
    }

    return block;
}

/*
 * Function:
 *      _bcm_kt_bfd_free_memory
 * Purpose:
 *      Free resource allocated for given unit.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      bfd_info   - (IN/OUT) Pointer to BFD info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_kt_bfd_free_memory(int unit, _bfd_info_t *bfd_info)
{
    if (bfd_info->endpoints != NULL) {
        sal_free(bfd_info->endpoints);
        bfd_info->endpoints = NULL;
    }

    if (bfd_info->endpoints_in_use != NULL) {
        sal_free(bfd_info->endpoints_in_use);
        bfd_info->endpoints_in_use = NULL;
    }

    if (bfd_info->auth_sha1 != NULL) {
        sal_free(bfd_info->auth_sha1);
        bfd_info->auth_sha1 = NULL;
    }

    if (bfd_info->auth_sp != NULL) {
        sal_free(bfd_info->auth_sp);
        bfd_info->auth_sp = NULL;
    }

    if (bfd_info->dma_buffer != NULL) {
        soc_cm_sfree(unit, bfd_info->dma_buffer);
        bfd_info->dma_buffer = NULL;
        bfd_info->dma_buffer_len = 0;
    }

}

/*
 * Function:
 *      _bcm_kt_bfd_event_unregister_all
 * Purpose:
 *      Free all event handlers.
 * Parameters:
 *      bfd_info - (IN/OUT) Pointer to BFD info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_kt_bfd_event_unregister_all(_bfd_info_t *bfd_info)
{
    _event_handler_t *event_handler;
    _event_handler_t *event_handler_to_delete;

    event_handler = bfd_info->event_handler_list;

    while (event_handler != NULL) {
        event_handler_to_delete = event_handler;
        event_handler = event_handler->next;

        sal_free(event_handler_to_delete);
    }

    bfd_info->event_handler_list = NULL;
}

/*
 * Function:
 *      _bfd_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      yc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bfd_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_BFD_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_BFD_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_BFD_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_BFD_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_BFD_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_BFD_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_BFD_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_BFD_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_BFD_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_BFD_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_BFD_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_BFD_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_BFD_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_BFD_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_BFD_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_BFD_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_BFD_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_BFD_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_BFD_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_kt_bfd_msg_send_receive
 * Purpose:
 *      Sends given BFD control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) BFD message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 *
 *     BFD Long Control message:
 *     - BFD control messages that require send/receive of information
 *       that cannot fit in the uc_msg 'len' and 'data' fields need to
 *       use DMA operations to exchange information (long control message).
 *
 *     - BFD convention for long control messages for
 *        'mos_msg_data_t' fields:
 *          'len'    size of the DMA buffer to send to uController
 *          'data'   physical DMA memory address to send or receive
 *
 *      DMA Operations:
 *      - DMA read/write operation is performed when a long BFD control
 *        message is involved.
 *
 *      - Messages that require DMA operation (long control message)
 *        is indicated by MOS_MSG_DMA_MSG().
 *
 *      - Callers must 'pack' and 'unpack' corresponding information
 *        into/from DMA buffer indicated by BFD_INFO(unit)->dma_buffer.
 *
 */
STATIC int
_bcm_kt_bfd_msg_send_receive(int unit, uint8 s_subclass,
                             uint16 s_len, uint32 s_data,
                             uint8 r_subclass, uint16 *r_len)
{
    int rv;
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    mos_msg_data_t send, reply;
    uint8 *dma_buffer;
    int dma_buffer_len;
    uint32 uc_rv;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_BFD;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    dma_buffer = bfd_info->dma_buffer;
    dma_buffer_len = bfd_info->dma_buffer_len;
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, bfd_info->uc_num,
                                      &send, &reply,
                                      _BFD_UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != MOS_MSG_CLASS_BFD) ||
        (reply.s.subclass != r_subclass)) {
        return BCM_E_INTERNAL;
    }

    /* Convert BFD uController error code to BCM */
    uc_rv = bcm_ntohl(reply.s.data);
    rv = _bfd_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    return rv;
}

/*
 * Function:
 *      _bcm_kt_bfd_event_mask_set
 * Purpose:
 *      Set the BFD Events mask.
 *      Events are set per BFD module.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_kt_bfd_event_mask_set(int unit)
{
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _event_handler_t *event_handler;
    uint32 event_mask = 0;
    uint16 reply_len;

    /* Get event mask from all callbacks */
    for (event_handler = bfd_info->event_handler_list;
         event_handler != NULL;
         event_handler = event_handler->next) {

        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventStateChange)) {
            event_mask |= BFD_BTE_EVENT_STATE;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventRemoteStateDiag)) {
            event_mask |= BFD_BTE_EVENT_REMOTE_STATE_DIAG;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventSessionDiscriminatorChange)) {
            event_mask |= BFD_BTE_EVENT_DISCRIMINATOR;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventAuthenticationChange)) {
            event_mask |= BFD_BTE_EVENT_AUTHENTICATION;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventParameterChange)) {
            event_mask |= BFD_BTE_EVENT_PARAMETER;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventSessionError)) {
            event_mask |= BFD_BTE_EVENT_ERROR;
        }
    }

    /* Update BFD event mask in uKernel */
    if (event_mask != bfd_info->event_mask) {
        /* Send BFD Event Mask message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_EVENT_MASK_SET,
              0, event_mask,
              MOS_MSG_SUBCLASS_BFD_EVENT_MASK_SET_REPLY,
              &reply_len));

        if (reply_len != 0) {
            return BCM_E_INTERNAL;
        }
    }

    bfd_info->event_mask = event_mask;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to BFD info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_kt_bfd_callback_thread(void *param)
{
    int rv;
    _bfd_info_t *bfd_info = param;
    _event_handler_t *event_handler;
    bcm_bfd_event_types_t events, cb_events;
    bfd_msg_event_t event_msg;
    uint16 sess_id;
    uint32 event_mask;
    int invoke = 0;

    bfd_info->event_thread_id   = sal_thread_self();
    bfd_info->event_thread_kill = 0;

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(bfd_info->unit, bfd_info->uc_num,
                                     MOS_MSG_CLASS_BFD_EVENT, &event_msg,
                                     sal_sem_FOREVER);

        if (BCM_FAILURE(rv) || (bfd_info->event_thread_kill)) {
            break;   /* Thread exit */
        }

        /* Get data from event message */
        sess_id    = bcm_ntohs(event_msg.s.len);
        event_mask = bcm_ntohl(event_msg.s.data);

        /* Set events */
        sal_memset(&events, 0, sizeof(events));
        if (event_mask & BFD_BTE_EVENT_STATE) {
            SHR_BITSET(events.w, bcmBFDEventStateChange);
        }
        if (event_mask & BFD_BTE_EVENT_REMOTE_STATE_DIAG) {
            SHR_BITSET(events.w, bcmBFDEventRemoteStateDiag);
        }
        if (event_mask & BFD_BTE_EVENT_DISCRIMINATOR) {
            SHR_BITSET(events.w, bcmBFDEventSessionDiscriminatorChange);
        }
        if (event_mask & BFD_BTE_EVENT_AUTHENTICATION) {
            SHR_BITSET(events.w, bcmBFDEventAuthenticationChange);
        }
        if (event_mask & BFD_BTE_EVENT_PARAMETER) {
            SHR_BITSET(events.w, bcmBFDEventParameterChange);
        }
        if (event_mask & BFD_BTE_EVENT_ERROR) {
            SHR_BITSET(events.w, bcmBFDEventSessionError);
        }


        /* Loop over registered callbacks,
         * If any match the events field, then invoke
         */
        sal_memset(&cb_events, 0, sizeof(cb_events));
        for (event_handler = bfd_info->event_handler_list;
             event_handler != NULL;
             event_handler = event_handler->next) {
            SHR_BITAND_RANGE(event_handler->event_types.w, events.w,
                             0, bcmBFDEventCount, cb_events.w);
            SHR_BITTEST_RANGE(cb_events.w, 0, bcmBFDEventCount, invoke);

            if (invoke) {
                
                event_handler->cb(bfd_info->unit, 0,
                                  cb_events, sess_id,
                                  event_handler->user_data);
            }
        }
    }

    bfd_info->event_thread_kill = 0;
    bfd_info->event_thread_id   = NULL;
    sal_thread_exit(0);
}

/*
 * Function:
 *      _bcm_kt_bfd_find_free_endpoint
 * Purpose:
 *      Find an available endpoint index (endpoint id).
 * Parameters:
 *      endpoints_in_use - Current endpoint ids usage bitmap.
 *      endpoint_count   - Max number of endpoints.
 * Returns:
 *      Available endpoint id.
 *      (-1) Indicates no more available endpoint ids.
 * Notes:
 */
STATIC int
_bcm_kt_bfd_find_free_endpoint(SHR_BITDCL *endpoints_in_use,
                               int endpoint_count)
{
    int endpoint_index;
    
    for (endpoint_index = 0; endpoint_index < endpoint_count;
         endpoint_index += 1) {
        if (!SHR_BITGET(endpoints_in_use, endpoint_index)) {
            break;
        }
    }

    if (endpoint_index >= endpoint_count) {
        endpoint_index = -1;
    }

    return endpoint_index;
}

/*
 * Function:
 *      _bcm_kt_bfd_find_ipv4_entry
 */
STATIC int
_bcm_kt_bfd_find_ipv4_entry(int unit, bcm_vrf_t vrf,
                            bcm_ip_t ip_addr, int *index,
                            l3_entry_ipv4_unicast_entry_t *l3_entry)
{
    l3_entry_ipv4_unicast_entry_t l3_key;

    sal_memset(&l3_key, 0, sizeof(l3_key));

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_key, VRF_IDf, vrf);

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_key, IP_ADDRf, ip_addr);

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_key, KEY_TYPEf,
                                           TR_L3_HASH_KEY_TYPE_V4UC);

    return soc_mem_search(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ANY,
                          index, &l3_key, l3_entry, 0);
}

/*
 * Function:
 *      _bcm_kt_bfd_find_ipv6_entry
 */
STATIC int
_bcm_kt_bfd_find_ipv6_entry(int unit, bcm_vrf_t vrf,
                            bcm_ip6_t ip6_addr,int *index,
                            l3_entry_ipv6_unicast_entry_t *l3_entry)
{
    l3_entry_ipv6_unicast_entry_t l3_key;

    sal_memset(&l3_key, 0, sizeof(l3_key));

    soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_UNICASTm,
                         &l3_key, IP_ADDR_LWR_64f,
                         ip6_addr, SOC_MEM_IP6_LOWER_ONLY);

    soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_UNICASTm,
                         &l3_key, IP_ADDR_UPR_64f,
                         ip6_addr, SOC_MEM_IP6_UPPER_ONLY);

    soc_L3_ENTRY_IPV6_UNICASTm_field32_set(unit, &l3_key,
                                           KEY_TYPE_1f,
                                           TR_L3_HASH_KEY_TYPE_V6UC);

    soc_L3_ENTRY_IPV6_UNICASTm_field32_set(unit, &l3_key,
                                           KEY_TYPE_0f,
                                           TR_L3_HASH_KEY_TYPE_V6UC);

    soc_L3_ENTRY_IPV6_UNICASTm_field32_set(unit,
                                           &l3_key, VRF_IDf, vrf);

    return soc_mem_search(unit, L3_ENTRY_IPV6_UNICASTm, MEM_BLOCK_ANY,
                          index, &l3_key, l3_entry, 0);

}

/*
 * Function:
 *      _bcm_kt_bfd_l2x_entry_set
 */
STATIC int
_bcm_kt_bfd_l2x_entry_set(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                          int mpls, int endpoint_index,
                          bcm_module_t module_id, bcm_port_t port_id,
                          int is_local)
{
    l2x_entry_t l2_entry;

    /* Insert your discriminator field to L2_ENTRY table. */
    sal_memset(&l2_entry, 0, sizeof(l2_entry));

    soc_mem_field32_set(unit, L2Xm, &l2_entry, KEY_TYPEf, 4);
    soc_mem_field32_set(unit, L2Xm, &l2_entry, VALIDf, 1);

    if (mpls) {
        soc_mem_field32_set(unit, L2Xm, &l2_entry, SESSION_IDENTIFIER_TYPEf,
                            1);
        soc_mem_field32_set(unit, L2Xm, &l2_entry, LABELf,
                            endpoint_info->label);
    } else {
        soc_mem_field32_set(unit, L2Xm, &l2_entry, SESSION_IDENTIFIER_TYPEf,
                            0);
        soc_mem_field32_set(unit, L2Xm, &l2_entry, YOUR_DISCRIMINATORf,
                            endpoint_info->local_discr);
    }

    if (!is_local) {
        soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_REMOTEf, 1);
        soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_DST_MODf,
                            module_id);
        soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_DST_PORTf,
                            port_id);
        soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_INT_PRIf,
                            endpoint_info->int_pri);
    } else {
        soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_RX_SESSION_INDEXf,
                            endpoint_index);
        soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_CPU_QUEUE_CLASSf,
                            endpoint_info->cpu_qid);
    }

    SOC_IF_ERROR_RETURN(soc_mem_insert(unit, L2Xm, MEM_BLOCK_ANY, &l2_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_l2x_entry_clear
 */
STATIC int
_bcm_kt_bfd_l2x_entry_clear(int unit,
                            int mpls, int mpls_label, int your_discr)
{
    l2x_entry_t l2_key;
    l2x_entry_t l2_entry;
    int l2_index;

    sal_memset(&l2_key, 0, sizeof(l2_key));

    soc_mem_field32_set(unit, L2Xm, &l2_key, KEY_TYPEf, 4);

    if (mpls) {
        soc_mem_field32_set(unit, L2Xm, &l2_key,
                            SESSION_IDENTIFIER_TYPEf, 1);
        soc_mem_field32_set(unit, L2Xm, &l2_key, LABELf, mpls_label);
    } else {
        soc_mem_field32_set(unit, L2Xm, &l2_key,
                            SESSION_IDENTIFIER_TYPEf, 0);
        soc_mem_field32_set(unit, L2Xm, &l2_key,
                            YOUR_DISCRIMINATORf, your_discr);
    }
    
    SOC_IF_ERROR_RETURN
        (soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                        &l2_key, &l2_entry, 0));

    SOC_IF_ERROR_RETURN
        (soc_mem_delete_index(unit, L2Xm, MEM_BLOCK_ANY, l2_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_l3_entry_set
 */
STATIC int
_bcm_kt_bfd_l3_entry_set(int unit,
                         bcm_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    int l3_index = 0;
    l3_entry_ipv4_unicast_entry_t l3_entry_v4;
    l3_entry_ipv6_unicast_entry_t l3_entry_v6;

    /* Check whether ipv4 or ipv6 */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_find_ipv6_entry(unit,
                                         endpoint_info->vrf_id,
                                         endpoint_info->src_ip6_addr,
                                         &l3_index, &l3_entry_v6));

        /* Set BFD_ENABLE and LOCAL_ADDRESS */
        rv = soc_mem_field32_modify(unit, L3_ENTRY_IPV6_UNICASTm,
                                    l3_index, LOCAL_ADDRESSf, 1);
        rv |= soc_mem_field32_modify(unit, L3_ENTRY_IPV6_UNICASTm,
                                     l3_index, BFD_ENABLEf, 1);
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_find_ipv4_entry(unit,
                                         endpoint_info->vrf_id,
                                         endpoint_info->src_ip_addr,
                                         &l3_index, &l3_entry_v4));
        /* Set BFD_ENABLE and LOCAL_ADDRESS */
        rv = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                                    l3_index, LOCAL_ADDRESSf, 1);
        rv |= soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                                     l3_index, BFD_ENABLEf, 1);
    }

    return rv;
}
                         
/*
 * Function:
 *      _bcm_kt_bfd_l3_entry_clear
 */
STATIC int
_bcm_kt_bfd_l3_entry_clear(int unit,
                           bcm_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    int l3_index = 0;
    l3_entry_ipv4_unicast_entry_t l3_entry_v4;
    l3_entry_ipv6_unicast_entry_t l3_entry_v6;

    /* Check whether ipv4 or ipv6 */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_find_ipv6_entry(unit,
                                         endpoint_info->vrf_id,
                                         endpoint_info->src_ip6_addr,
                                         &l3_index, &l3_entry_v6));
        /* Reset BFD_ENABLE and LOCAL_ADDRESS */
        rv = soc_mem_field32_modify(unit, L3_ENTRY_IPV6_UNICASTm,
                                    l3_index, LOCAL_ADDRESSf, 0);
        rv |= soc_mem_field32_modify(unit, L3_ENTRY_IPV6_UNICASTm,
                                     l3_index, BFD_ENABLEf, 0);
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_find_ipv4_entry(unit,
                                         endpoint_info->vrf_id,
                                         endpoint_info->src_ip_addr,
                                         &l3_index, &l3_entry_v4));
        /* Reset BFD_ENABLE and LOCAL_ADDRESS */
        rv = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                                    l3_index, LOCAL_ADDRESSf, 0);
        rv |= soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                                     l3_index, BFD_ENABLEf, 0);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_kt_bfd_find_l3_tunnel_entry
 */
STATIC int
_bcm_kt_bfd_find_l3_tunnel_entry(int unit,
                                 bcm_bfd_endpoint_info_t *endpoint_info,
                                 soc_tunnel_term_t *tunnel_entry)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    bcm_tunnel_type_t tunnel_type;
    bcm_tunnel_terminator_t tunnel;
    soc_tunnel_term_t key;

    /* Tunnel type */
    if (endpoint_info->type == bcmBFDTunnelTypeIp4in4) {
        tunnel_type = bcmTunnelTypeIp4In4;
    } else if (endpoint_info->type == bcmBFDTunnelTypeIp6in4) {
        tunnel_type = bcmTunnelTypeIp6In4;
    } else if (endpoint_info->type == bcmBFDTunnelTypeIp4in6) {
        tunnel_type = bcmTunnelTypeIp4In6;
    } else if (endpoint_info->type == bcmBFDTunnelTypeIp6in6) {
        tunnel_type = bcmTunnelTypeIp6In6;
    } else if (endpoint_info->type == bcmBFDTunnelTypeGre4In4) {
        tunnel_type = bcmTunnelTypeGre4In4;
    } else if (endpoint_info->type == bcmBFDTunnelTypeGre6In4) {
        tunnel_type = bcmTunnelTypeGre6In4;
    } else if (endpoint_info->type == bcmBFDTunnelTypeGre4In6) {
        tunnel_type = bcmTunnelTypeGre4In6;
    } else if (endpoint_info->type == bcmBFDTunnelTypeGre6In6) {
        tunnel_type = bcmTunnelTypeGre6In6;
    } else {
        return BCM_E_PARAM;
    }

    /* Initialize lookup key */
    bcm_tunnel_terminator_t_init(&tunnel);
    tunnel.type = tunnel_type;
    if ((tunnel_type == bcmTunnelTypeIp4In4) ||
        (tunnel_type == bcmTunnelTypeIp6In4) ||
        (tunnel_type == bcmTunnelTypeGre4In4) ||
        (tunnel_type == bcmTunnelTypeGre6In4)) {
        tunnel.sip  = endpoint_info->dst_ip_addr;
        tunnel.dip  = endpoint_info->src_ip_addr;
        tunnel.sip_mask = 0xffffffff;
        tunnel.dip_mask = 0xffffffff;
    } else {
        sal_memcpy(tunnel.sip6, endpoint_info->dst_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(tunnel.dip6, endpoint_info->src_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memset(tunnel.sip6_mask, 0xff, BCM_IP6_ADDRLEN);
        sal_memset(tunnel.dip6_mask, 0xff, BCM_IP6_ADDRLEN);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_l3_tnl_term_entry_init(unit, &tunnel, &key));

    /* Find tunnel terminator in TCAM */
    BCM_IF_ERROR_RETURN(soc_tunnel_term_match(unit, &key, tunnel_entry));

    return BCM_E_NONE;

#else
    return BCM_E_UNAVAIL;
#endif
}

STATIC int
_bcm_kt_bfd_l3_tunnel_entry_set(int unit,
                                bcm_bfd_endpoint_info_t *endpoint_info)
{
    soc_tunnel_term_t tunnel_entry;
    uint32 *tunnel_entry_ptr;
    uint32 tunnel_index;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_find_l3_tunnel_entry(unit, endpoint_info,
                                          &tunnel_entry));

    tunnel_entry_ptr = (uint32 *)&tunnel_entry.entry_arr[0];
    soc_L3_TUNNELm_field32_set(unit, tunnel_entry_ptr, BFD_ENABLEf, 1);

    BCM_IF_ERROR_RETURN
        (soc_tunnel_term_insert(unit, &tunnel_entry, &tunnel_index));

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_l3_tunnel_entry_clear(int unit,
                                bcm_bfd_endpoint_info_t *endpoint_info)
{
    soc_tunnel_term_t tunnel_entry;
    uint32 *tunnel_entry_ptr;
    uint32 tunnel_index;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_find_l3_tunnel_entry(unit, endpoint_info,
                                          &tunnel_entry));

    tunnel_entry_ptr = (uint32 *)&tunnel_entry.entry_arr[0];
    soc_L3_TUNNELm_field32_set(unit, tunnel_entry_ptr, BFD_ENABLEf, 0);

    BCM_IF_ERROR_RETURN
        (soc_tunnel_term_insert(unit, &tunnel_entry, &tunnel_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_mpls_entry_set
 */
STATIC int
_bcm_kt_bfd_mpls_entry_set(int unit,
                           bcm_bfd_endpoint_info_t *endpoint_info)
{
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
    mpls_entry_entry_t mpls_entry;
    mpls_entry_entry_t mpls_key;
    int mpls_index = 0;
    int i, num_entries;
    bcm_l3_egress_t l3_egress;
    int ingress_if;
    int cc_type = 0;
    int cw_check = 0;
    
    sal_memset(&mpls_key, 0, sizeof(mpls_key));
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_key,
                                MPLS_LABELf, endpoint_info->label);

    SOC_IF_ERROR_RETURN(soc_mem_search(unit, MPLS_ENTRYm,
                                       MEM_BLOCK_ANY, &mpls_index,
                                       &mpls_key, &mpls_entry, 0));

    if ((soc_MPLS_ENTRYm_field32_get(unit,
                                     &mpls_entry, VALIDf) != 0x1)) {
        return BCM_E_PARAM;
    }

    /* Set CW Check if Control Word is present */
    if ((endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) ||
        (endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv) ||
        (endpoint_info->type == bcmBFDTunnelTypePweControlWord) ||
        ((endpoint_info->type == bcmBFDTunnelTypePweRouterAlert) &&
         (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH)) ||
        ((endpoint_info->type == bcmBFDTunnelTypePweTtl) &&
         (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH))) {
        cw_check = 1;
    }

    /* Set PW CC Type */
    if (endpoint_info->type == bcmBFDTunnelTypePweControlWord) {
        cc_type = 1;
    } else if (endpoint_info->type == bcmBFDTunnelTypePweRouterAlert) {
        cc_type = 2;
    } else if (endpoint_info->type == bcmBFDTunnelTypePweTtl) {
        cc_type = 3;
    }

    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry,
                                SESSION_IDENTIFIER_TYPEf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, BFD_ENABLEf, 1);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, CW_CHECK_CTRLf,
                                cw_check);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, PW_CC_TYPEf, cc_type);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, MPLS_ENTRYm,
                                      MEM_BLOCK_ANY, mpls_index,
                                      &mpls_entry));

    /*
     * PW CC-3
     *
     * Set MPLS entry DECAP_USE_TTL=0 for corresponding
     * Tunnel Terminator label.
     */
    if (endpoint_info->type == bcmBFDTunnelTypePweTtl) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_l3_egress_get(unit, endpoint_info->egress_if,
                                   &l3_egress));

        /* Look for Tunnel Terminator label */
        num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
        for (i = 0; i < num_entries; i++) {
            BCM_IF_ERROR_RETURN
                (READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &mpls_entry));
            if (!soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, VALIDf)) {
                continue;
            }
            if (soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                            MPLS_ACTION_IF_BOSf) == 0x1) {
                continue;    /* L2_SVP */
            }

            ingress_if = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                                     L3_IIFf);
            if (ingress_if == l3_egress.intf) {
                /* Label found */
                soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry,
                                            DECAP_USE_TTLf, 0);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, MPLS_ENTRYm,
                                                  MEM_BLOCK_ALL, i,
                                                  &mpls_entry));
                break;
            }
        }
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH_SUPPORT && BCM_MPLS_SUPPORT */
}

/*
 * Function:
 *      _bcm_kt_bfd_mpls_entry_clear
 */
STATIC int
_bcm_kt_bfd_mpls_entry_clear(int unit,
                             bcm_bfd_endpoint_info_t *endpoint_info)
{
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
    mpls_entry_entry_t mpls_entry;
    mpls_entry_entry_t mpls_key;
    int mpls_index = 0;
    
    sal_memset(&mpls_key, 0, sizeof(mpls_key));
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_key,
                                MPLS_LABELf, endpoint_info->label);

    SOC_IF_ERROR_RETURN(soc_mem_search(unit, MPLS_ENTRYm,
                                       MEM_BLOCK_ANY, &mpls_index,
                                       &mpls_key, &mpls_entry, 0));

    if ((soc_MPLS_ENTRYm_field32_get(unit,
                                     &mpls_entry, VALIDf) != 0x1)) {
        return BCM_E_PARAM;
    }

    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, BFD_ENABLEf, 0);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, PW_CC_TYPEf, 0);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, MPLS_ENTRYm,
                                      MEM_BLOCK_ANY,
                                      mpls_index, &mpls_entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH_SUPPORT && BCM_MPLS_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_hw_init
 * Purpose:
 *      Initialize the HW for BFD packet processing.
 *      Configure:
 *      - Copy to CPU BFD error packets
 *      - Assign RX DMA channel to BFD CPU COS Queue
 *      - Map RX BFD packets to BFD CPU COS Queue
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt_bfd_hw_init(int unit)
{
    int rv = BCM_E_NONE;
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    uint32 val;
    int index;
    int slowpath_index, ach_err_index;
    int cosq_map_size;
    bcm_rx_reasons_t reasons, reasons_mask;
    uint8 int_prio, int_prio_mask;
    uint32 packet_type, packet_type_mask;
    bcm_cos_queue_t cosq;
    bcm_rx_chan_t chan_id;
    int num_cosq = 0;
    int min_cosq, max_cosq;
    int i;
    l2x_entry_t l2_entry;

    /*
     * Send BFD error packet to CPU
     *
     * Configure CPU_CONTROL_0 register
     */
    BCM_IF_ERROR_RETURN(READ_CPU_CONTROL_0r(unit, &val));
    soc_reg_field_set(unit, CPU_CONTROL_0r, &val,
                      BFD_UNKNOWN_VERSION_TOCPUf, 1);
    soc_reg_field_set(unit, CPU_CONTROL_0r, &val,
                      BFD_YOUR_DISCRIMINATOR_NOT_FOUND_TOCPUf, 1);
    soc_reg_field_set(unit, CPU_CONTROL_0r, &val,
                      BFD_UNKNOWN_CONTROL_PACKET_TOCPUf, 1);
    soc_reg_field_set(unit, CPU_CONTROL_0r, &val,
                      BFD_UNKNOWN_ACH_CHANNEL_TYPE_TOCPUf, 1);
    soc_reg_field_set(unit, CPU_CONTROL_0r, &val,
                      BFD_UNKNOWN_ACH_VERSION_TOCPUf, 1);
    BCM_IF_ERROR_RETURN(WRITE_CPU_CONTROL_0r(unit, val));

    /*
     * Set BFD ACH Channel Types
     */
    BCM_IF_ERROR_RETURN(READ_BFD_RX_ACH_TYPE_CONTROL0r(unit, &val));
    soc_reg_field_set(unit, BFD_RX_ACH_TYPE_CONTROL0r, &val,
                      BFD_ACH_TYPE_IPV4f, SHR_BFD_ACH_CHANNEL_TYPE_IPV4);
    soc_reg_field_set(unit, BFD_RX_ACH_TYPE_CONTROL0r, &val,
                      BFD_ACH_TYPE_IPV6f, SHR_BFD_ACH_CHANNEL_TYPE_IPV6);
    BCM_IF_ERROR_RETURN(WRITE_BFD_RX_ACH_TYPE_CONTROL0r(unit, val));

    BCM_IF_ERROR_RETURN(READ_BFD_RX_ACH_TYPE_CONTROL1r(unit, &val));
    soc_reg_field_set(unit, BFD_RX_ACH_TYPE_CONTROL1r, &val,
                      BFD_ACH_TYPE_RAWf, SHR_BFD_ACH_CHANNEL_TYPE_RAW);
    BCM_IF_ERROR_RETURN(WRITE_BFD_RX_ACH_TYPE_CONTROL1r(unit, val));

    BCM_IF_ERROR_RETURN(READ_BFD_RX_ACH_TYPE_MPLSTPr(unit, &val));
    soc_reg_field_set(unit, BFD_RX_ACH_TYPE_MPLSTPr, &val,
                      BFD_ACH_TYPE_MPLSTP_CCf,
                      SHR_BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC);
    soc_reg_field_set(unit, BFD_RX_ACH_TYPE_MPLSTPr, &val,
                      BFD_ACH_TYPE_MPLSTP_CVf,
                      SHR_BFD_ACH_CHANNEL_TYPE_MPLS_TP_CV);
    BCM_IF_ERROR_RETURN(WRITE_BFD_RX_ACH_TYPE_MPLSTPr(unit, val));


    /*
     * Set special Session-ID for Unknown Remote Discriminator
     *
     * This allows initial BFD Control packets, where the remote
     * discriminator is unknown (your_discriminator=0),
     * to be received as 'good packets' and be distinct from
     * other errors (wrong BFD version) that may result
     * in "bcmRxReasonBfdLookupFailure"
     */
    sal_memset(&l2_entry, 0, sizeof(l2_entry));
    soc_mem_field32_set(unit, L2Xm, &l2_entry, KEY_TYPEf, 4);
    soc_mem_field32_set(unit, L2Xm, &l2_entry, VALIDf, 1);
    soc_mem_field32_set(unit, L2Xm, &l2_entry, SESSION_IDENTIFIER_TYPEf, 0);
    soc_mem_field32_set(unit, L2Xm, &l2_entry, YOUR_DISCRIMINATORf,
                        SHR_BFD_DISCR_UNKNOWN_YOUR_DISCR);
    soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_RX_SESSION_INDEXf,
                        SHR_BFD_DISCR_UNKNOWN_SESSION_ID);
    soc_mem_field32_set(unit, L2Xm, &l2_entry, BFD_CPU_QUEUE_CLASSf, 0);
    SOC_IF_ERROR_RETURN(soc_mem_insert(unit, L2Xm, MEM_BLOCK_ANY, &l2_entry));


    /*
     * BFD COS Queue
     *
     * BFD COSQ will be set to one of the following:
     * - User configured BFD COSq, if provided
     * - The default BFD COS queue.  This is set to the highest queue
     *   available within the queue range assigned to the uC where
     *   the BFD application is running.
     */

    /* Get valid range of COS queues for uC where BFD is loaded */
    min_cosq = 0;
    for (i = 0; i < SOC_CMCS_NUM(unit); i++) {
        num_cosq = NUM_CPU_ARM_COSQ(unit, i);
        if (i == bfd_info->uc_num + 1) {
            break;
        }
        min_cosq += num_cosq;
    }
    max_cosq = min_cosq + num_cosq - 1;

    /* Check that there is at least one COS queue assigned to the CMC */
    if (max_cosq < min_cosq) {
        BCM_ERR(("ERROR: No BFD COS Queue available from uC%d\n",
                 bfd_info->uc_num));
        return BCM_E_CONFIG;
    }

    /* Check user provide COSQ */
    if (bfd_info->cpu_cosq != BFD_COSQ_INVALID) {
        if ((bfd_info->cpu_cosq < min_cosq) ||
            (bfd_info->cpu_cosq > max_cosq)) {
            return BCM_E_CONFIG;
        }
        min_cosq = max_cosq = bfd_info->cpu_cosq;
    }


    /*
     * Assign RX DMA channel to BFD CPU COS Queue
     * (This is the RX channel to listen on for BFD packets).
     *
     * DMA channels (12) are assigned 4 per processor:
     * (see /src/bcm/common/rx.c)
     *   channels 0..3  --> PCI host
     *   channels 4..7  --> uController 0
     *   chnanels 8..11 --> uController 1
     *
     * The uControllers designate the 4 local DMA channels as follows:
     *   local channel  0     --> TX
     *   local channel  1..3  --> RX
     *
     * Each uController application needs to use a different
     * RX DMA channel to listen on.
     */
    chan_id = (BCM_RX_CHANNELS * (SOC_ARM_CMC(unit, bfd_info->uc_num))) +
        bfd_info->rx_channel;
    for (i = max_cosq; i >= min_cosq; i--) {
        rv = _bcm_common_rx_queue_channel_set(unit, i, chan_id);
        if (BCM_SUCCESS(rv)) {
            bfd_info->cpu_cosq = i;  /* COSQ found */
            break;
        }
    }
    if (i < min_cosq) {
        BCM_ERR(("ERROR: No BFD COS Queue available from uC%d\n",
                 bfd_info->uc_num));
        return BCM_E_RESOURCE;    /* Could not find an available COSQ */
    }


    /*
     * Direct BFD packets to designated RX CPU COS Queue
     *
     * Reasons:
     *   - bcmRxReasonBfdSlowpath
     *         BFD session lookup hit (good packet)
     *   - bcmRxReasonBfd / bcmRxReasonBfdUnknownVersion
     *         BFD error - Unknown ACH Version or Channel Type
     * NOTE:
     *     BFD RX reasons need to be mapped using different indexes
     *     due to HW restriction.
     *
     *     The user input 'cpu_qid' (bcm_bfd_endpoint_t) could be
     *     used to select different CPU COS queue. Currently,
     *     all priorities are mapped into the same CPU COS Queue.
     */

    /* Find available entries in CPU COS queue map table */
    slowpath_index   = -1;   /* COSQ map index for good packets */
    ach_err_index    = -1;   /* COSQ map index for unknown ACH error */
    BCM_IF_ERROR_RETURN
        (bcm_esw_rx_cosq_mapping_size_get(unit, &cosq_map_size));
    
    for (index = 0; index < cosq_map_size; index++) {
        rv = bcm_esw_rx_cosq_mapping_get(unit, index,
                                         &reasons, &reasons_mask,
                                         &int_prio, &int_prio_mask,
                                         &packet_type, &packet_type_mask,
                                         &cosq);
        if (rv == BCM_E_NOT_FOUND) {
            /* Find available indexes for BFD reason codes */
            rv = BCM_E_NONE;
            if (slowpath_index == -1) {
                slowpath_index = index;
            } else {
                ach_err_index = index;
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    if ((slowpath_index == -1) || (ach_err_index == -1)) {
        return BCM_E_RESOURCE;
    }

    /*
     * Map RX reason code to RX CPU COS Queue
     */

    /* BFD Slowpath - Lookup hit */
    BCM_RX_REASON_CLEAR_ALL(reasons);
    BCM_RX_REASON_SET(reasons, bcmRxReasonBfdSlowpath);  /* BFD hit, good */
    BCM_IF_ERROR_RETURN
        (bcm_esw_rx_cosq_mapping_set(unit, slowpath_index,
                                     reasons, reasons,
                                     0, 0, /* Any priority */
                                     0, 0, /* Any packet type */
                                     bfd_info->cpu_cosq));
    bfd_info->cosq_spath_index = slowpath_index;

    /* BFD Error - ACH Unknown Version or Channel Type */
    BCM_RX_REASON_CLEAR_ALL(reasons);
    BCM_RX_REASON_SET(reasons, bcmRxReasonBfd);               /* BFD error */
    BCM_RX_REASON_SET(reasons, bcmRxReasonBfdUnknownVersion); /* ACH unknown */

    BCM_IF_ERROR_RETURN
        (bcm_esw_rx_cosq_mapping_set(unit, ach_err_index,
                                     reasons, reasons,
                                     0, 0, /* Any priority */
                                     0, 0, /* Any packet type */
                                     bfd_info->cpu_cosq));
    bfd_info->cosq_ach_err_index = ach_err_index;

    return rv;
}

/*
 * Functions:
 *      _bcm_kt_bfd_<header/label>_pack
 * Purpose:
 *      The following set of _pack() functions packs in
 *      network byte order a given header/label.
 * Parameters:
 *      buffer          - (OUT) Buffer where to pack header/label.
 *      <header/label>  - (IN) Header/label to pack.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 */
STATIC uint8 *
_bcm_kt_bfd_udp_header_pack(uint8 *buffer, _udp_header_t *udp)
{
    _BFD_ENCAP_PACK_U16(buffer, udp->src);
    _BFD_ENCAP_PACK_U16(buffer, udp->dst);
    _BFD_ENCAP_PACK_U16(buffer, udp->length);
    _BFD_ENCAP_PACK_U16(buffer, udp->sum);

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_ipv4_header_pack(uint8 *buffer, _ipv4_header_t *ip)
{
    uint32  tmp;

    tmp = ((ip->version & 0xf) << 28) | ((ip->h_length & 0xf) << 24) |
        ((ip->dscp & 0x3f) << 18) | ((ip->ecn & 0x3)<< 16) | (ip->length);
    _BFD_ENCAP_PACK_U32(buffer, tmp);

    tmp = (ip->id << 16) | ((ip->flags & 0x7) << 13) | (ip->f_offset & 0x1fff);
    _BFD_ENCAP_PACK_U32(buffer, tmp);

    tmp = (ip->ttl << 24) | (ip->protocol << 16) | ip->sum;
    _BFD_ENCAP_PACK_U32(buffer, tmp);

    _BFD_ENCAP_PACK_U32(buffer, ip->src);
    _BFD_ENCAP_PACK_U32(buffer, ip->dst);

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_ipv6_header_pack(uint8 *buffer, _ipv6_header_t *ip)
{
    uint32  tmp;
    int     i;

    tmp = ((ip->version & 0xf) << 28) | (ip->t_class << 20) |
        (ip->f_label & 0xfffff);
    _BFD_ENCAP_PACK_U32(buffer, tmp);

    tmp = (ip->p_length << 16 ) | (ip->next_header << 8) | ip->hop_limit;
    _BFD_ENCAP_PACK_U32(buffer, tmp);

    for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
        _BFD_ENCAP_PACK_U8(buffer, ip->src[i]);
    }

    for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
        _BFD_ENCAP_PACK_U8(buffer, ip->dst[i]);
    }

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_gre_header_pack(uint8 *buffer, _gre_header_t *gre)
{
    uint32  tmp;

    tmp = ((gre->checksum & 0x1) << 31) | ((gre->routing & 0x1) << 30) |
        ((gre->key & 0x1) << 29) | ((gre->sequence_num & 0x1) << 28) | 
        ((gre->strict_src_route & 0x1) << 27) |
        ((gre->recursion_control & 0x7) << 24) | ((gre->flags & 0x1f) << 19) |
        ((gre->version & 0x7) << 16) | (gre->protocol & 0xffff);

    _BFD_ENCAP_PACK_U32(buffer, tmp);

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_ach_header_pack(uint8 *buffer, _ach_header_t *ach)
{
    uint32  tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) | ((ach->version & 0xf) << 24) |
        (ach->reserved << 16) | ach->channel_type;

    _BFD_ENCAP_PACK_U32(buffer, tmp);

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_mpls_label_pack(uint8 *buffer, _mpls_label_t *mpls)
{
    uint32  tmp;

    tmp = ((mpls->label & 0xfffff) << 12) | ((mpls->exp & 0x7) << 9) |
        ((mpls->s & 0x1) << 8) | mpls->ttl;
    _BFD_ENCAP_PACK_U32(buffer, tmp);

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_l2_header_pack(uint8 *buffer, _l2_header_t *l2)
{
    uint32  tmp;
    int     i;

    for (i = 0; i < _BFD_MAC_ADDR_LENGTH; i++) {
        _BFD_ENCAP_PACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < _BFD_MAC_ADDR_LENGTH; i++) {
        _BFD_ENCAP_PACK_U8(buffer, l2->src_mac[i]);
    }

    /*
     * VLAN Tag
     *
     * A TPID value of 0 indicates this is an untagged frame.
     */
    if (l2->vlan_tag.tpid != 0) {
        /* Add VLAN tag */
        tmp = (l2->vlan_tag.tpid << 16) |
            ((l2->vlan_tag.tci.prio & 0x7) << 13) |
            ((l2->vlan_tag.tci.cfi & 0x1) << 12) |
            (l2->vlan_tag.tci.vid & 0xfff);
        _BFD_ENCAP_PACK_U32(buffer, tmp);
    }

    _BFD_ENCAP_PACK_U16(buffer, l2->etype);

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_hg_header_pack(uint8 *buffer, int unit,
                           _endpoint_config_t *endpoint_config,
                           _l2_header_t *l2)
{
    soc_higig_hdr_t *hg = (soc_higig_hdr_t *)buffer;
    int hg_size;
    int src_mod;
    int src_port;
    int dst_mod;
    int dst_port;
    uint32 vlan_val;
    uint32 prio_val;
    uint32 pfm;

    hg_size = sizeof(*hg);

    if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &src_mod))) {
        return NULL;
    }
    src_port = endpoint_config->tx_port;
    dst_mod  = endpoint_config->modid;
    dst_port = endpoint_config->port;
    vlan_val = l2->vlan_tag.tci.vid;
    pfm      = SOC_DEFAULT_DMA_PFM_GET(unit);
    prio_val = endpoint_config->info.int_pri;

    sal_memset(hg, 0, hg_size);
    soc_higig_field_set(unit, hg, HG_start, SOC_HIGIG_START);
    soc_higig_field_set(unit, hg, HG_hgi, SOC_HIGIG_HGI);
    soc_higig_field_set(unit, hg, HG_opcode, SOC_HIGIG_OP_UC);
    soc_higig_field_set(unit, hg, HG_hdr_format, SOC_HIGIG_HDR_DEFAULT);
    soc_higig_field_set(unit, hg, HG_vlan_tag, vlan_val);
    soc_higig_field_set(unit, hg, HG_dst_mod, dst_mod);
    soc_higig_field_set(unit, hg, HG_dst_port, dst_port);
    soc_higig_field_set(unit, hg, HG_src_mod, src_mod);
    soc_higig_field_set(unit, hg, HG_src_port, src_port);
    soc_higig_field_set(unit, hg, HG_pfm, pfm);
    soc_higig_field_set(unit, hg, HG_cos, prio_val);

    buffer += hg_size;

    return buffer;
}

STATIC uint8 *
_bcm_kt_bfd_hg2_header_pack(uint8 *buffer, int unit,
                           _endpoint_config_t *endpoint_config,
                           _l2_header_t *l2)
{
    soc_higig2_hdr_t *hg = (soc_higig2_hdr_t *)buffer;
    int hg_size;
    int src_mod;
    int src_port;
    int dst_mod;
    int dst_port;
    uint32 vlan_val;
    uint32 prio_val;
    uint32 pfm;
    uint32 ppd;

    hg_size = sizeof(*hg);

    if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &src_mod))) {
        return NULL;
    }
    src_port = endpoint_config->tx_port;
    dst_mod  = endpoint_config->modid;
    dst_port = endpoint_config->port;
    vlan_val = l2->vlan_tag.tci.vid;
    pfm      = SOC_DEFAULT_DMA_PFM_GET(unit);
    prio_val = endpoint_config->info.int_pri;
    ppd      = 0;

    sal_memset(hg, 0, hg_size);
    soc_higig2_field_set(unit, hg, HG_start, SOC_HIGIG2_START);
    soc_higig2_field_set(unit, hg, HG_ppd_type, ppd);
    soc_higig2_field_set(unit, hg, HG_opcode, SOC_HIGIG_OP_UC);
    soc_higig2_field_set(unit, hg, HG_vlan_tag, vlan_val);
    soc_higig2_field_set(unit, hg, HG_dst_mod, dst_mod);
    soc_higig2_field_set(unit, hg, HG_dst_port, dst_port);
    soc_higig2_field_set(unit, hg, HG_src_mod, src_mod);
    soc_higig2_field_set(unit, hg, HG_src_port, src_port);
    soc_higig2_field_set(unit, hg, HG_pfm, pfm);
    soc_higig2_field_set(unit, hg, HG_tc, prio_val);

    buffer += hg_size;

    return buffer;
}

/*
 * Function:
 *      _bcm_kt_bfd_udp_checksum_set
 * Purpose:
 *      Set the UDP initial checksum (excludes checksum for the data).
 *
 *      The checksum includes the IP pseudo-header and UDP header.
 *      It does not include the checksum for the data (BFD packet).
 *      The data checksum will be added to UDP initial checksum in
 *      the uC side each time a packet is sent, since data payload may vary.
 * Parameters:
 *      v6              - (IN) Set if IP is IPv6.
 *      ipv4            - (IN) IPv4 header.
 *      ipv6            - (IN) IPv6 header.
 *      udp             - (IN/OUT) UDP header checksum to update.
 * Returns:
 *      None
 */
STATIC void
_bcm_kt_bfd_udp_checksum_set(int v6,
                             _ipv4_header_t *ipv4, _ipv6_header_t *ipv6,
                             _udp_header_t *udp)
{
    uint8  buffer[SHR_BFD_UDP_HEADER_LENGTH + (BCM_IP6_ADDRLEN*2) + 8];
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
            _BFD_ENCAP_PACK_U8(cur_ptr, ipv6->src[i]);
        }
        for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
            _BFD_ENCAP_PACK_U8(cur_ptr, ipv6->dst[i]);
        }
        _BFD_ENCAP_PACK_U32(cur_ptr, udp->length);
        _BFD_ENCAP_PACK_U32(cur_ptr, ipv6->next_header);
    } else {
        /* IPv4 Pseudo-Header
         *     Source address
         *     Destination address
         *     Protocol    (lower 8 bits of 16-bit)
         *     UDP length  (16-bit)
         */
        _BFD_ENCAP_PACK_U32(cur_ptr, ipv4->src);
        _BFD_ENCAP_PACK_U32(cur_ptr, ipv4->dst);
        _BFD_ENCAP_PACK_U16(cur_ptr, ipv4->protocol);
        _BFD_ENCAP_PACK_U16(cur_ptr, udp->length);
    }

    /* Add UDP header */
    cur_ptr = _bcm_kt_bfd_udp_header_pack(cur_ptr, udp);

    /* Calculate initial UDP checksum */
    length = cur_ptr - buffer;
    udp->sum = _shr_ip_chksum(length, buffer);
}

/*
 * Functions:
 *      _bcm_kt_bfd_<header/label>_get
 * Purpose:
 *      The following set of _get() functions builds a given header/label.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN) Pointer to BFD endpoint structure.
 *      packet_flags    - (IN) Flags for building packet.
 *      <... other IN args ...>
 *      <header/label>  - (OUT) Returns header/label.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt_bfd_udp_header_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                           uint32 packet_flags, _udp_header_t *udp)
{
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    int auth_length;
    int bfd_length;

    /* Calculate BFD packet length */
    /* Authentication (optional) */
    switch (endpoint_info->auth) {
    case _SHR_BFD_AUTH_TYPE_SIMPLE_PASSWORD:
        auth_length = SHR_BFD_AUTH_SP_HEADER_START_LENGTH +
            bfd_info->auth_sp[endpoint_info->auth_index].length;
        break;
    case _SHR_BFD_AUTH_TYPE_KEYED_SHA1:
    case _SHR_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1:
        auth_length = SHR_BFD_AUTH_SHA1_LENGTH;
        break;
    default:
        auth_length = 0;
        break;
    }
    bfd_length = SHR_BFD_CONTROL_HEADER_LENGTH + auth_length;

    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = endpoint_info->udp_src_port;
    udp->dst = (packet_flags & _BFD_ENCAP_PKT_UDP__MULTI_HOP) ?
        SHR_BFD_UDP_MULTI_HOP_DEST_PORT : SHR_BFD_UDP_SINGLE_HOP_DEST_PORT;
    udp->length = SHR_BFD_UDP_HEADER_LENGTH + bfd_length;
    udp->sum = 0;    /* Calculated later */

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_ipv4_header_get(int data_length, uint8 protocol,
                            uint8 tos, uint8 ttl,
                            bcm_ip_t src_ip_addr, bcm_ip_t dst_ip_addr,
                            _ipv4_header_t *ip)
{
    uint8 buffer[SHR_BFD_IPV4_HEADER_LENGTH];

    sal_memset(ip, 0, sizeof(*ip));

    ip->version  = SHR_BFD_IPV4_VERSION;
    ip->h_length = SHR_BFD_IPV4_HEADER_LENGTH_WORDS;
    ip->dscp     = tos;
    ip->ecn      = 0;
    ip->length   = SHR_BFD_IPV4_HEADER_LENGTH + data_length;
    ip->id       = 0;
    ip->flags    = 0;
    ip->f_offset = 0;
    ip->ttl      = ttl;
    ip->protocol = protocol;
    ip->sum      = 0;
    ip->src      = src_ip_addr;
    ip->dst      = dst_ip_addr;

    /* Calculate IP header checksum */
    _bcm_kt_bfd_ipv4_header_pack(buffer, ip);
    ip->sum = _shr_ip_chksum(SHR_BFD_IPV4_HEADER_LENGTH, buffer);

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_ipv6_header_get(int data_length, uint8 protocol,
                            uint8 tos, uint8 ttl,
                            bcm_ip6_t src_ip_addr, bcm_ip6_t dst_ip_addr,
                            _ipv6_header_t *ip)
{
    sal_memset(ip, 0, sizeof(*ip));

    ip->version     = SHR_BFD_IPV6_VERSION;
    ip->t_class     = tos << 2;
    ip->f_label     = 0;  /* TODO: MPLS, Tunnel */
    ip->p_length    = data_length;
    ip->next_header = protocol;
    ip->hop_limit   = ttl;
    sal_memcpy(ip->src, src_ip_addr, BCM_IP6_ADDRLEN);
    sal_memcpy(ip->dst, dst_ip_addr, BCM_IP6_ADDRLEN);

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_ip_headers_get(bcm_bfd_endpoint_info_t *endpoint_info,
                           uint32 packet_flags,
                           int udp_length,
                           uint16 *etype,
                           _ipv4_header_t *ipv4, _ipv6_header_t *ipv6,
                           _ipv4_header_t *inner_ipv4,
                           _ipv6_header_t *inner_ipv6)
{
    int ip_protocol;
    int ip_length;

    if (!(packet_flags & _BFD_ENCAP_PKT_IP)) {
        return BCM_E_PARAM;
    }

    /* Inner IP Header (IP-in-IP/GRE) */
    if (packet_flags & _BFD_ENCAP_PKT_INNER_IP) {

        if (packet_flags & _BFD_ENCAP_PKT_INNER_IP__V6) {
            ip_protocol = SHR_BFD_IP_PROTOCOL_IPV6_ENCAP;
            ip_length   = udp_length + SHR_BFD_IPV6_HEADER_LENGTH;
            BCM_IF_ERROR_RETURN
                (_bcm_kt_bfd_ipv6_header_get(udp_length,
                                             SHR_BFD_IP_PROTOCOL_UDP,
                                             endpoint_info->inner_ip_tos,
                                             endpoint_info->inner_ip_ttl,
                                             endpoint_info->inner_src_ip6_addr,
                                             endpoint_info->inner_dst_ip6_addr,
                                             inner_ipv6));
        } else {
            ip_protocol = SHR_BFD_IP_PROTOCOL_IPV4_ENCAP;
            ip_length   = udp_length + SHR_BFD_IPV4_HEADER_LENGTH;
            BCM_IF_ERROR_RETURN
                (_bcm_kt_bfd_ipv4_header_get(udp_length,
                                             SHR_BFD_IP_PROTOCOL_UDP,
                                             endpoint_info->inner_ip_tos,
                                             endpoint_info->inner_ip_ttl,
                                             endpoint_info->inner_src_ip_addr,
                                             endpoint_info->inner_dst_ip_addr,
                                             inner_ipv4));
        }
    } else {
        ip_protocol = SHR_BFD_IP_PROTOCOL_UDP;
        ip_length   = udp_length;
    }

    /* If GRE is present, GRE the protocol that follows the outer IP header */
    if (packet_flags & _BFD_ENCAP_PKT_GRE) {
        ip_protocol = SHR_BFD_IP_PROTOCOL_GRE;
        ip_length   = udp_length + SHR_BFD_GRE_HEADER_LENGTH;
        if (packet_flags & _BFD_ENCAP_PKT_INNER_IP__V6) {
            ip_length += SHR_BFD_IPV6_HEADER_LENGTH;
        } else {
            ip_length += SHR_BFD_IPV4_HEADER_LENGTH;
        }
    }

    /* Outer IP Header */
    if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
        *etype = SHR_BFD_L2_ETYPE_IPV6;
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_ipv6_header_get(ip_length,
                                         ip_protocol,
                                         endpoint_info->ip_tos,
                                         endpoint_info->ip_ttl,
                                         endpoint_info->src_ip6_addr,
                                         endpoint_info->dst_ip6_addr,
                                         ipv6));
    } else {
        *etype = SHR_BFD_L2_ETYPE_IPV4;
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_ipv4_header_get(ip_length,
                                         ip_protocol,
                                         endpoint_info->ip_tos,
                                         endpoint_info->ip_ttl,
                                         endpoint_info->src_ip_addr,
                                         endpoint_info->dst_ip_addr,
                                         ipv4));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_gre_header_get(uint32 packet_flags, _gre_header_t *gre)
{
    sal_memset(gre, 0, sizeof(*gre));

    gre->checksum          = 0;
    gre->routing           = 0;
    gre->key               = 0;
    gre->sequence_num      = 0;
    gre->strict_src_route  = 0;
    gre->recursion_control = 0;
    gre->flags             = 0;
    gre->version           = 0;
    if (packet_flags & _BFD_ENCAP_PKT_INNER_IP__V6) {
        gre->protocol = SHR_BFD_L2_ETYPE_IPV6;
    } else {
        gre->protocol = SHR_BFD_L2_ETYPE_IPV4;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_ach_header_get(uint32 packet_flags, _ach_header_t *ach)
{
    sal_memset(ach, 0, sizeof(*ach));

    ach->f_nibble = SHR_BFD_ACH_FIRST_NIBBLE;
    ach->version  = SHR_BFD_ACH_VERSION;
    ach->reserved = 0;

    if (packet_flags & _BFD_ENCAP_PKT_G_ACH__IP) {    /* UDP/IP */
        if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
            ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_IPV6;
        } else {
            ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_IPV4;
        }

    } else if (packet_flags & _BFD_ENCAP_PKT_G_ACH__CC) { /* MPLS-TP CC only */
        ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC;

    } else if (packet_flags & _BFD_ENCAP_PKT_G_ACH__CC_CV) { /* MPLS-TP CC/CV */
        /*
         * CC/CV is done by interleaving CC and CV PDUs.
         * Set CC channel type as a start.
         * BFD engine (uKernel) will set appropriate channel types.
         * One CV PDU is inserted every second.
         */
        ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC;

    } else {    /* Raw */
        ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_RAW;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_mpls_label_get(uint32 label, uint8 exp, uint8 s, uint8 ttl,
                           _mpls_label_t *mpls)
{
    sal_memset(mpls, 0, sizeof(*mpls));

    mpls->label = label;
    mpls->exp   = exp;
    mpls->s     = s;
    mpls->ttl   = ttl;  /* Can ttl be zero?? */

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_bfd_mpls_gal_label_get(_mpls_label_t *mpls)
{
    return _bcm_kt_bfd_mpls_label_get(SHR_BFD_MPLS_GAL_LABEL,
                                      0, 1, 1, mpls);
}

STATIC int
_bcm_kt_bfd_mpls_router_alert_label_get(_mpls_label_t *mpls)
{
    return _bcm_kt_bfd_mpls_label_get(SHR_BFD_MPLS_ROUTER_ALERT_LABEL,
                                      0, 0, 1, mpls);
}

STATIC int
_bcm_kt_bfd_mpls_labels_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                            uint32 packet_flags,
                            int max_count, _mpls_label_t *mpls,
                            int *mpls_count)
{
    int count = 0;
    bcm_l3_egress_t l3_egress;
    bcm_mpls_port_t mpls_port;
    bcm_mpls_egress_label_t label_array[_BFD_MPLS_MAX_LABELS];
    int label_count;
    int i;

    /* Get MPLS Tunnel Label(s) */
    bcm_l3_egress_t_init(&l3_egress);
    if (BCM_FAILURE(bcm_esw_l3_egress_get(unit, endpoint_info->egress_if,
                                          &l3_egress))) {
        return BCM_E_PARAM;
    }

    /* Look for a tunnel associated with this interface */
    if (BCM_SUCCESS
        (bcm_esw_mpls_tunnel_initiator_get(unit, l3_egress.intf,
                                           _BFD_MPLS_MAX_LABELS,
                                           label_array, &label_count))) {
        for (i = 0; (i < label_count) && (count < max_count); i++) {
            _bcm_kt_bfd_mpls_label_get(label_array[i].label,
                                       label_array[i].exp,
                                       0,
                                       label_array[i].ttl,
                                       &mpls[count++]);
        }
    }

    /* MPLS Router Alert */
    if (packet_flags & _BFD_ENCAP_PKT_MPLS_ROUTER_ALERT) {
        _bcm_kt_bfd_mpls_router_alert_label_get(&mpls[count++]);
    }

    /* Get L2 VPN PW VC label */
    if (BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
        /* Get mpls port and label info */
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.mpls_port_id = endpoint_info->gport;
        if (BCM_FAILURE
            (bcm_esw_mpls_port_get(unit, endpoint_info->vpn,
                                   &mpls_port))) {
            return BCM_E_PARAM;
        } else {
            if (endpoint_info->type == bcmBFDTunnelTypePweTtl) {
                mpls_port.egress_label.ttl = 0x1;
            }

            _bcm_kt_bfd_mpls_label_get(mpls_port.egress_label.label,
                                       mpls_port.egress_label.exp,
                                       0,
                                       mpls_port.egress_label.ttl,
                                       &mpls[count++]);
        }
    }

    /* Set Bottom of Stack if there is no GAL label */
    if (!(packet_flags & _BFD_ENCAP_PKT_GAL)) {
        mpls[count-1].s = 1;
    }

    *mpls_count = count;

    return BCM_E_NONE;

}

STATIC int
_bcm_kt_bfd_l2_header_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                          uint32 packet_flags, bcm_port_t port,
                          uint16 etype, _l2_header_t *l2)
{
    uint16 tpid;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
    bcm_vlan_t vid;

    sal_memset(l2, 0, sizeof(*l2));

    /* Get L3 interfaces */
    bcm_l3_egress_t_init(&l3_egress);
    bcm_l3_intf_t_init(&l3_intf);

    if (BCM_FAILURE
        (bcm_esw_l3_egress_get(unit, endpoint_info->egress_if, &l3_egress))) {
        return BCM_E_PARAM;
    }

    l3_intf.l3a_intf_id = l3_egress.intf;
    if (BCM_FAILURE(bcm_esw_l3_intf_get(unit, &l3_intf))) {
        return BCM_E_PARAM;
    }

    /* Get TPID */
    if (BCM_FAILURE(bcm_esw_port_tpid_get(unit, port, &tpid))) {
        return BCM_E_INTERNAL;
    }

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, l3_egress.mac_addr, _BFD_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, l3_intf.l3a_mac_addr, _BFD_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag
     *
     * Skip VLAN tag if port's untagged VLAN matches the given VLAN,
     * or if HG header is present.
     */
    l2->vlan_tag.tpid     = tpid;
    l2->vlan_tag.tci.prio = endpoint_info->vlan_pri;
    l2->vlan_tag.tci.cfi  = 0;
    l2->vlan_tag.tci.vid  = l3_intf.l3a_vid;
    if (BCM_SUCCESS(bcm_esw_port_untagged_vlan_get(unit, port, &vid))) {
        if ((vid == l2->vlan_tag.tci.vid) ||
            (packet_flags & (_BFD_ENCAP_PKT_HG | _BFD_ENCAP_PKT_HG2))) {
            l2->vlan_tag.tpid = 0;  /* Set to 0 to indicate untagged */
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_encap_build_pack
 * Purpose:
 *      Builds and packs the BFD packet encapsulation for a given
 *      BFD tunnel type.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN/OUT) Pointer to BFD endpoint structure.
 *      packet_flags    - (IN) Flags for building packet.
 *      buffer          - (OUT) Buffer returning BFD encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BFD encapsulation includes only all the
 *      encapsulation headers/labels and does not include
 *      the BFD control packet.
 */
STATIC int
_bcm_kt_bfd_encap_build_pack(int unit,
                             _endpoint_config_t *endpoint_config,
                             uint32 packet_flags,
                             uint8 *buffer)
{
    bcm_bfd_endpoint_info_t *endpoint_info = &endpoint_config->info;
    uint8          *cur_ptr = buffer;
    bcm_port_t     tx_port;  /* Local port to TX packet */
    uint16         etype = 0;
    _udp_header_t  udp;
    _ipv4_header_t ipv4, inner_ipv4;
    _ipv6_header_t ipv6, inner_ipv6;
    _gre_header_t  gre;
    int            v6;
    _ach_header_t  ach;
    _mpls_label_t  mpls_gal;
    _mpls_label_t  mpls_labels[_BFD_MPLS_MAX_LABELS];
    int            mpls_count;
    int            i;
    _l2_header_t   l2;
    int            ip_offset = 0;

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));

    tx_port = endpoint_config->tx_port;

    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if (packet_flags & _BFD_ENCAP_PKT_UDP) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_udp_header_get(unit, endpoint_info,
                                        packet_flags, &udp));
    }

    if (packet_flags & _BFD_ENCAP_PKT_IP) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_ip_headers_get(endpoint_info,
                                        packet_flags, udp.length,
                                        &etype,
                                        &ipv4, &ipv6,
                                        &inner_ipv4, &inner_ipv6));
    }

    /* Set UDP checksum with corresponding IP header information */
    if (packet_flags & _BFD_ENCAP_PKT_UDP) {
        v6 = 0;
        if (packet_flags & _BFD_ENCAP_PKT_INNER_IP) {
            if (packet_flags & _BFD_ENCAP_PKT_INNER_IP__V6) {
                v6 = 1;
            }
            _bcm_kt_bfd_udp_checksum_set(v6, &inner_ipv4, &inner_ipv6, &udp);
        } else {
            if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
                v6 = 1;
            }
            _bcm_kt_bfd_udp_checksum_set(v6, &ipv4, &ipv6, &udp);
        }
    }

    if (packet_flags & _BFD_ENCAP_PKT_GRE) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_gre_header_get(packet_flags, &gre));
    }

    if (packet_flags & _BFD_ENCAP_PKT_G_ACH) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_ach_header_get(packet_flags, &ach));
    }

    if (packet_flags & _BFD_ENCAP_PKT_GAL) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_mpls_gal_label_get(&mpls_gal));
    }

    if (packet_flags & _BFD_ENCAP_PKT_MPLS) {
        etype = SHR_BFD_L2_ETYPE_MPLS_UCAST;
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_mpls_labels_get(unit, endpoint_info,
                                         packet_flags,
                                         _BFD_MPLS_MAX_LABELS,
                                         mpls_labels, &mpls_count));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_l2_header_get(unit, endpoint_info, packet_flags,
                                   tx_port, etype, &l2));


    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* HG Header */
    if (packet_flags & _BFD_ENCAP_PKT_HG) {
        cur_ptr = _bcm_kt_bfd_hg_header_pack(cur_ptr, unit,
                                             endpoint_config, &l2);
    } else if (packet_flags & _BFD_ENCAP_PKT_HG2) {
        cur_ptr = _bcm_kt_bfd_hg2_header_pack(cur_ptr, unit,
                                              endpoint_config, &l2);
    }
    if (cur_ptr == NULL) {
        return BCM_E_INTERNAL;
    }

    /* L2 Header is always present */
    cur_ptr = _bcm_kt_bfd_l2_header_pack(cur_ptr, &l2);

    if (packet_flags & _BFD_ENCAP_PKT_MPLS) {
        for (i = 0; i < mpls_count; i++) {
            cur_ptr = _bcm_kt_bfd_mpls_label_pack(cur_ptr, &mpls_labels[i]);
        }
    }

    /* Remaining packet headers */
    if (packet_flags & _BFD_ENCAP_PKT_GAL) {
        cur_ptr = _bcm_kt_bfd_mpls_label_pack(cur_ptr, &mpls_gal);
    }
    if (packet_flags & _BFD_ENCAP_PKT_G_ACH) {
        cur_ptr = _bcm_kt_bfd_ach_header_pack(cur_ptr, &ach);
    }
    if (packet_flags & _BFD_ENCAP_PKT_GRE) {
    }
    if (packet_flags & _BFD_ENCAP_PKT_IP) {
        /*
         * IP offset for Lookup Key
         * For IP-in-IP, this is the outer IP header
         */
        ip_offset = cur_ptr - buffer;
        if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
            cur_ptr = _bcm_kt_bfd_ipv6_header_pack(cur_ptr, &ipv6);
        } else {
            cur_ptr = _bcm_kt_bfd_ipv4_header_pack(cur_ptr, &ipv4);
        }
    }
    if (packet_flags & _BFD_ENCAP_PKT_GRE) {
        cur_ptr = _bcm_kt_bfd_gre_header_pack(cur_ptr, &gre);
    }
    if (packet_flags & _BFD_ENCAP_PKT_INNER_IP) {
        if (packet_flags & _BFD_ENCAP_PKT_INNER_IP__V6) {
            cur_ptr = _bcm_kt_bfd_ipv6_header_pack(cur_ptr, &inner_ipv6);
        } else {
            cur_ptr = _bcm_kt_bfd_ipv4_header_pack(cur_ptr, &inner_ipv4);
        }
    }
    if (packet_flags & _BFD_ENCAP_PKT_UDP) {
        cur_ptr = _bcm_kt_bfd_udp_header_pack(cur_ptr, &udp);
    }


    /* Set BFD encapsulation length */
    endpoint_config->encap_length = cur_ptr - buffer;

    /*
     * Set Lookup Key for initial BFD packets
     * Offset is relative to the start of a given encapsulation data.
     *
     * Lookup Key:
     *   UDP/IP  : IPSA of RX packet       = IPDA on BFD Session
     *   IP-in-IP: Outer IPSA of RX packet = Outer IPDA on BFD Session
     *   MPLS    : MPLS BOS of RX packet   = 'label' on BFD endpoint struct
     *             - If GAL label (BOS) is present, then use MPLS label above.
     *             - 'mpls_label' on BFD endpoint is the incomming inner MPLS
     *               label.
     */
    endpoint_config->lkey_etype  = etype;
    endpoint_config->lkey_offset = 0;
    endpoint_config->lkey_length = 0;

    if (etype == SHR_BFD_L2_ETYPE_IPV4) {
        endpoint_config->lkey_offset = ip_offset +
            SHR_BFD_IPV4_HEADER_DA_OFFSET;
        endpoint_config->lkey_length = SHR_BFD_IPV4_HEADER_SA_LENGTH;

    } else if (etype ==  SHR_BFD_L2_ETYPE_IPV6) {
        endpoint_config->lkey_offset = ip_offset +
            SHR_BFD_IPV6_HEADER_DA_OFFSET;
        endpoint_config->lkey_length = SHR_BFD_IPV6_HEADER_SA_LENGTH;

    } else if (etype == SHR_BFD_L2_ETYPE_MPLS_UCAST) {
        /*
         * The lookup key is the incomming inner MPLS label,
         * which is specified when the BFD endpoint is create.
         *
         * The lookup key data is located in the 'mpls_label'
         * field of the message, rather than in the encap data
         * packet buffer.
         */
        endpoint_config->lkey_offset = 0;
        endpoint_config->lkey_length = SHR_BFD_MPLS_LABEL_LABEL_LENGTH;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_encap_data_dump
 * Purpose:
 *      Dumps buffer contents.
 * Parameters:
 *      buffer  - (IN) Buffer to dump data.
 *      length  - (IN) Length of buffer.
 * Returns:
 *      None
 */
void
_bcm_kt_bfd_encap_data_dump(uint8 *buffer, int length)
{
    int i;

    soc_cm_print("\nBFD encapsulation (length=%d):\n", length);

    for (i = 0; i < length; i++) {
        if ((i % 10) == 0) {
            soc_cm_print("\n");
        }
        soc_cm_print(" 0x%02x", buffer[i]);
    }

    soc_cm_print("\n");
    return;
}

/*
 * Function:
 *      _bcm_kt_bfd_ip_validate_1
 * Purpose:
 *      Performs the following IP address check and IP TTL configuration:
 *      - IP Source Address is routable
 *      - IP Destination Addresss is routable
 *      - Sets IP TTL=255  for 1-hop
 * Parameters:
 *      endpoint_info - (IN/OUT) Pointer to BFD endpoint info structure.
 *      packet_flags  - (IN) Packet flags.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_bfd_ip_validate_1(bcm_bfd_endpoint_info_t *endpoint_info,
                          uint32 packet_flags)
{

    /* IP-SA and IP-DA Routable */
    if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
        ENCAP_IPV6_ROUTABLE_VALIDATE(endpoint_info->src_ip6_addr);
        ENCAP_IPV6_ROUTABLE_VALIDATE(endpoint_info->dst_ip6_addr);
        if (BCM_IP6_MULTICAST(endpoint_info->src_ip6_addr)) {
            return BCM_E_PARAM;
        }
        if (IPV6_LOOPBACK(endpoint_info->dst_ip6_addr)) {
            return BCM_E_PARAM;
        }
    } else {
        ENCAP_IPV4_ROUTABLE_VALIDATE(endpoint_info->src_ip_addr);
        ENCAP_IPV4_ROUTABLE_VALIDATE(endpoint_info->dst_ip_addr);
        if (BCM_IP4_MULTICAST(endpoint_info->src_ip_addr)) {
            return BCM_E_PARAM;
        }
        if (IPV4_LOOPBACK(endpoint_info->dst_ip_addr)) {
            return BCM_E_PARAM;
        }
    }

    if (!(packet_flags & _BFD_ENCAP_PKT_UDP__MULTI_HOP)) {
        endpoint_info->ip_ttl = 255;    /* 1-hop */
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_bfd_ip_validate_2
 * Purpose:
 *      Performs the following IP address check and IP TTL configuration:
 *      - IP Source Address is routable
 *      - IP Destination Addresss is loopback or routable address
 *      - Sets IP TTL=1  for 1-hop when IP DA is loopback,
 *        or   IP TTL=255 for 1-hop when IP DA is a routable address         
 * Parameters:
 *      endpoint_info - (IN/OUT) Pointer to BFD endpoint info structure.
 *      packet_flags  - (IN) Packet flags.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_bfd_ip_validate_2(bcm_bfd_endpoint_info_t *endpoint_info,
                          uint32 packet_flags)
{
    int ip_ttl = 0;
    bcm_ip_t src_ip_addr, dst_ip_addr;
    bcm_ip6_t src_ip6_addr, dst_ip6_addr;
    int v6 = 0;

    if (packet_flags & _BFD_ENCAP_PKT_INNER_IP) {
        if (packet_flags & _BFD_ENCAP_PKT_INNER_IP__V6) {
            v6 = 1;
        }
        src_ip_addr  = endpoint_info->inner_src_ip_addr;
        dst_ip_addr  = endpoint_info->inner_dst_ip_addr;
        sal_memcpy(src_ip6_addr, endpoint_info->inner_src_ip6_addr,
                   BCM_IP6_ADDRLEN);
        sal_memcpy(dst_ip6_addr, endpoint_info->inner_dst_ip6_addr,
                   BCM_IP6_ADDRLEN);
    } else {
        if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
            v6 = 1;
        }
        src_ip_addr  = endpoint_info->src_ip_addr;
        dst_ip_addr  = endpoint_info->dst_ip_addr;
        sal_memcpy(src_ip6_addr, endpoint_info->src_ip6_addr,
                   BCM_IP6_ADDRLEN);
        sal_memcpy(dst_ip6_addr, endpoint_info->dst_ip6_addr,
                   BCM_IP6_ADDRLEN);
    }

    /* IP-SA Routable */
    if (v6) {
        ENCAP_IPV6_ROUTABLE_VALIDATE(src_ip6_addr);
    } else {
        ENCAP_IPV4_ROUTABLE_VALIDATE(src_ip_addr);
    }

    /* IP-DA:
     *     Loopback: 127/8 or 0:0:0:0:0:FFFF:127/104, then ttl=1
     *     Routable: then ttl=255
     */
    if (v6) {
        if (IPV6_LOOPBACK(dst_ip6_addr)) {
            ip_ttl = 1;
        } else {
            ENCAP_IPV6_ROUTABLE_VALIDATE(dst_ip6_addr);
            ip_ttl = 255;
        }
    } else {
        if (IPV4_LOOPBACK(dst_ip_addr)) {
            ip_ttl = 1;
        } else {
            ENCAP_IPV4_ROUTABLE_VALIDATE(dst_ip_addr);
            ip_ttl = 255;
        }
    }

    if (!(packet_flags & _BFD_ENCAP_PKT_UDP__MULTI_HOP)) {
        /* 1-hop */
        if (packet_flags & _BFD_ENCAP_PKT_INNER_IP) {
            endpoint_info->inner_ip_ttl = ip_ttl;
        } else {
            endpoint_info->ip_ttl = ip_ttl;
        }            
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_kt_bfd_encap_create
 * Purpose:
 *      Creates a BFD packet encapsulation.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN/OUT) Pointer to BFD endpoint structure.
 *      encap_data      - (OUT) Buffer returning BFD encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BFD encapsulation buffer includes all the
 *      corresponding headers/labels EXCEPT for the BFD control packet.
 */
STATIC int
_bcm_kt_bfd_encap_create(int unit,
                         _endpoint_config_t *endpoint_config,
                         uint8 *encap_data)
{
    bcm_bfd_endpoint_info_t *endpoint_info;
    uint32 packet_flags;
    uint32 ip_flag;
    bcm_port_t tx_port;  /* Local port to TX packet */
    bcm_pbmp_t st_pbmp;
    bcm_pbmp_t tx_pbmp;

    endpoint_info = &endpoint_config->info;
    tx_port       = endpoint_config->tx_port;

    /* Check for incompatible flags */
    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW) &&
        (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6)) {
        return BCM_E_PARAM;
    }

    packet_flags = 0;
    ip_flag      = 0;

    /* Set IPv4 or IPv6 flag */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) {
        ip_flag |= _BFD_ENCAP_PKT_IP__V6;
    }

    /* 1-hop or Multi-hop */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        packet_flags |= _BFD_ENCAP_PKT_UDP__MULTI_HOP;
    }

    /*
     * Get BFD encapsulation packet format flags
     *
     * Also, perform the following for each BFD tunnel type:
     * - Check for valid parameter values
     * - Set specific values required by the BFD tunnel definition 
     *   (e.g. such as ttl=1,...)
     */
    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        packet_flags |= 
            (_BFD_ENCAP_PKT_IP | ip_flag |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeIp4in4:
        packet_flags |= 
            (_BFD_ENCAP_PKT_IP |
             _BFD_ENCAP_PKT_INNER_IP |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;


    case bcmBFDTunnelTypeIp6in4:
        packet_flags |= 
            (_BFD_ENCAP_PKT_IP |
             _BFD_ENCAP_PKT_INNER_IP | _BFD_ENCAP_PKT_INNER_IP__V6 |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeIp4in6:
        packet_flags |= 
            (_BFD_ENCAP_PKT_IP | _BFD_ENCAP_PKT_IP__V6 |
             _BFD_ENCAP_PKT_INNER_IP |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeIp6in6:
        packet_flags |= 
            (_BFD_ENCAP_PKT_IP | _BFD_ENCAP_PKT_IP__V6 |
             _BFD_ENCAP_PKT_INNER_IP | _BFD_ENCAP_PKT_INNER_IP__V6 |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeGre4In4:
        /*
         * bcmBFDTunnelTypeGRE is an old enum, which is now equivalent
         * to the new GRE IPv4-in-IPv4 tunnel type bcmBFDTunnelTypeGre4In4.
         */
        packet_flags |=
            (_BFD_ENCAP_PKT_GRE |
             _BFD_ENCAP_PKT_IP |
             _BFD_ENCAP_PKT_INNER_IP |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeGre6In4:
        packet_flags |=
            (_BFD_ENCAP_PKT_GRE |
             _BFD_ENCAP_PKT_IP |
             _BFD_ENCAP_PKT_INNER_IP | _BFD_ENCAP_PKT_INNER_IP__V6 |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeGre4In6:
        packet_flags |=
            (_BFD_ENCAP_PKT_GRE |
             _BFD_ENCAP_PKT_IP | _BFD_ENCAP_PKT_IP__V6 |
             _BFD_ENCAP_PKT_INNER_IP |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeGre6In6:

        packet_flags |=
            (_BFD_ENCAP_PKT_GRE |
             _BFD_ENCAP_PKT_IP | _BFD_ENCAP_PKT_IP__V6 |
             _BFD_ENCAP_PKT_INNER_IP | _BFD_ENCAP_PKT_INNER_IP__V6 |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_1(endpoint_info,
                                                      packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));
        break;

    case bcmBFDTunnelTypeMpls:
        packet_flags |=
            (_BFD_ENCAP_PKT_MPLS |
             _BFD_ENCAP_PKT_IP | ip_flag |
             _BFD_ENCAP_PKT_UDP);

        ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                      packet_flags));

        break;

    case bcmBFDTunnelTypeMplsTpCc:
        packet_flags |=
            (_BFD_ENCAP_PKT_MPLS |
             _BFD_ENCAP_PKT_GAL |
             _BFD_ENCAP_PKT_G_ACH |
             _BFD_ENCAP_PKT_G_ACH__CC);

        break;

    case bcmBFDTunnelTypeMplsTpCcCv:
        packet_flags |=
            (_BFD_ENCAP_PKT_MPLS |
             _BFD_ENCAP_PKT_GAL |
             _BFD_ENCAP_PKT_G_ACH |
             _BFD_ENCAP_PKT_G_ACH__CC_CV);

        if ((endpoint_info->mep_id_length <= 0) ||
            (endpoint_info->mep_id_length >
             _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH)) {
            return BCM_E_PARAM;
        }
        break;

    case bcmBFDTunnelTypePweControlWord:
        packet_flags |=
            (_BFD_ENCAP_PKT_MPLS |
             _BFD_ENCAP_PKT_G_ACH);

        /* Check for incompatible flags */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND) ||
            (endpoint_info->flags & BCM_BFD_ENDPOINT_PASSIVE)) {
            return BCM_E_PARAM;
        }

        /* PW required */
        if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
            return BCM_E_PARAM;
        }

        /* Raw mode or UDP/IP */
        if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW)) {
            /* UDP/IP */
            ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);
            packet_flags |= _BFD_ENCAP_PKT_G_ACH__IP;
            packet_flags |=
                (_BFD_ENCAP_PKT_IP | ip_flag |
                 _BFD_ENCAP_PKT_UDP);

            BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                          packet_flags));
        }
        break;

    case bcmBFDTunnelTypePweRouterAlert:
        packet_flags |= _BFD_ENCAP_PKT_MPLS_ROUTER_ALERT;
        /* Fall through */
    case bcmBFDTunnelTypePweTtl:
        packet_flags |=
            _BFD_ENCAP_PKT_MPLS;

        /* Check for incompatible flags */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND) ||
            (endpoint_info->flags & BCM_BFD_ENDPOINT_PASSIVE)) {
            return BCM_E_PARAM;
        }

        /* PW required */
        if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
            return BCM_E_PARAM;
        }

        /*
         * If ACH is present, UDP/IP and RAW can be used
         * Otherwise, only UDP/IP can be set
         */
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH) {
            packet_flags |= _BFD_ENCAP_PKT_G_ACH;
            /* Raw or UDP/IP */
            if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW)) {
                packet_flags |= _BFD_ENCAP_PKT_G_ACH__IP;
            }
        } else {
            if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW) {
                return BCM_E_PARAM;
            }
            packet_flags |= _BFD_ENCAP_PKT_G_ACH__IP;
        }
        
        if (packet_flags & _BFD_ENCAP_PKT_G_ACH__IP) {
           /* UDP/IP */
            ENCAP_UDP_SRCPORT_VALIDATE(endpoint_info->udp_src_port);
            packet_flags |= _BFD_ENCAP_PKT_G_ACH__IP;
            packet_flags |=
                (_BFD_ENCAP_PKT_IP | ip_flag |
                 _BFD_ENCAP_PKT_UDP);

            BCM_IF_ERROR_RETURN(_bcm_kt_bfd_ip_validate_2(endpoint_info,
                                                          packet_flags));
        }
        break;

    default:
        return BCM_E_PARAM;
    }

    /* Check if HG Header is needed (TX port is HG) */
    SOC_PBMP_PORT_SET(tx_pbmp, tx_port);
    SOC_PBMP_ASSIGN(st_pbmp, PBMP_ST_ALL(unit));
    BCM_API_XLATE_PORT_PBMP_P2A(unit, &st_pbmp);
    SOC_PBMP_AND(st_pbmp, tx_pbmp);
    if (SOC_PBMP_NOT_NULL(st_pbmp)) {
        if (IS_HG2_ENABLED_PORT(unit, tx_port)) {
            packet_flags |= _BFD_ENCAP_PKT_HG2;
        } else {
            packet_flags |= _BFD_ENCAP_PKT_HG;
        }
    }

    /* Build header/labels and pack in buffer */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_encap_build_pack(unit,
                                      endpoint_config,
                                      packet_flags,
                                      encap_data));

    /*
     * Set encap type.  Used to indicate uC for special
     * handling on certain encapsulation types:
     * - UDP checksum update is required
     * - packet is MPLS-TP CC only
     * - packet is MPLS-TP CC/CV
     */
    if (packet_flags & _BFD_ENCAP_PKT_UDP) {
        if (packet_flags & _BFD_ENCAP_PKT_IP__V6) {
            endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_V6UDP;
        } else {
            endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_V4UDP;
        }
    } else if (packet_flags & _BFD_ENCAP_PKT_G_ACH__CC) {
        endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_MPLS_TP_CC;
    } else if (packet_flags & _BFD_ENCAP_PKT_G_ACH__CC_CV) {
        endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_MPLS_TP_CC_CV;
    } else {
        endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_RAW;
    }                    

#ifdef _BFD_DEBUG_DUMP
    _bcm_kt_bfd_encap_data_dump(encap_data, endpoint_config->encap_length);
#endif

    return BCM_E_NONE;
}
    
/*
 * Function:
 *      _bcm_kt_bfd_session_hw_set
 * Purpose:
 *      Sets BFD Session in HW device.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN) Pointer to BFD endpoint structure.
 *      module_id       - (IN) Module id.
 *      port_id         - (IN) Port.
 *      is_local        - (IN) Indicates if module id is local.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt_bfd_session_hw_set(int unit, _endpoint_config_t *endpoint_config,
                           bcm_module_t module_id, bcm_port_t port_id,
                           int is_local)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *endpoint_info;
    int endpoint_index;

    endpoint_info  = &endpoint_config->info;
    endpoint_index = endpoint_config->endpoint_index;

    switch(endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        /* Set L2_ENTRY entry, Your Discriminator lookup */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_l2x_entry_set(unit, endpoint_info,
                                       0, endpoint_index,
                                       module_id, port_id, is_local));

        /* L3 entry */
        _bcm_esw_l3_lock(unit);
        rv = _bcm_kt_bfd_l3_entry_set(unit, endpoint_info);
        _bcm_esw_l3_unlock(unit);

        break;

    case bcmBFDTunnelTypeIp4in4:
    case bcmBFDTunnelTypeIp6in4:
    case bcmBFDTunnelTypeIp4in6:
    case bcmBFDTunnelTypeIp6in6:
    case bcmBFDTunnelTypeGre4In4:
    case bcmBFDTunnelTypeGre6In4:
    case bcmBFDTunnelTypeGre4In6:
    case bcmBFDTunnelTypeGre6In6:
        /* Set L2_ENTRY entry, Your Discriminator lookup */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_l2x_entry_set(unit, endpoint_info,
                                       0, endpoint_index,
                                       module_id, port_id, is_local));
        /* L3_TUNNEL */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_l3_tunnel_entry_set(unit, endpoint_info));

        break;

    case bcmBFDTunnelTypeMpls:
    case bcmBFDTunnelTypeMplsTpCc:
    case bcmBFDTunnelTypeMplsTpCcCv:
    case bcmBFDTunnelTypePweControlWord:
    case bcmBFDTunnelTypePweRouterAlert:
    case bcmBFDTunnelTypePweTtl:
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
        /* Set L2_ENTRY entry, MPLS lookup */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_l2x_entry_set(unit, endpoint_info,
                                       1, endpoint_index,
                                       module_id, port_id, is_local));

        /* MPLS entry */
        SOC_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
        rv = _bcm_kt_bfd_mpls_entry_set(unit, endpoint_info);
        bcm_tr_mpls_unlock (unit);
#else
        return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH_SUPPORT && BCM_MPLS_SUPPORT */
        break;

    default:
        rv = BCM_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_kt_bfd_session_hw_delete
 * Purpose:
 *      Delete BFD Session in HW device.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN) Pointer to BFD endpoint structure.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt_bfd_session_hw_delete(int unit, _endpoint_config_t *endpoint_config)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *endpoint_info;

    endpoint_info  = &endpoint_config->info;

    switch(endpoint_config->info.type) {
    case bcmBFDTunnelTypeUdp:
        
        /* L3 entry */
        _bcm_esw_l3_lock(unit);
        rv = _bcm_kt_bfd_l3_entry_clear(unit, endpoint_info);
        _bcm_esw_l3_unlock(unit);

        /* L2_ENTRY */
        rv = _bcm_kt_bfd_l2x_entry_clear(unit, 0,
                                         endpoint_info->label,
                                         endpoint_info->local_discr);
        break;

    case bcmBFDTunnelTypeIp4in4:
    case bcmBFDTunnelTypeIp6in4:
    case bcmBFDTunnelTypeIp4in6:
    case bcmBFDTunnelTypeIp6in6:
    case bcmBFDTunnelTypeGre4In4:
    case bcmBFDTunnelTypeGre6In4:
    case bcmBFDTunnelTypeGre4In6:
    case bcmBFDTunnelTypeGre6In6:
        /* L3_TUNNEL */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_l3_tunnel_entry_clear(unit, endpoint_info));

        /* L2_ENTRY */
        rv = _bcm_kt_bfd_l2x_entry_clear(unit, 0,
                                         endpoint_info->label,
                                         endpoint_info->local_discr);
        break;

    case bcmBFDTunnelTypeMpls:
    case bcmBFDTunnelTypeMplsTpCc:
    case bcmBFDTunnelTypeMplsTpCcCv:
    case bcmBFDTunnelTypePweControlWord:
    case bcmBFDTunnelTypePweRouterAlert:
    case bcmBFDTunnelTypePweTtl:
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
        /* MPLS entry */
        SOC_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
        rv = _bcm_kt_bfd_mpls_entry_clear(unit, endpoint_info);
        bcm_tr_mpls_unlock (unit);
        BCM_IF_ERROR_RETURN(rv);

        /* L2_ENTRY */
        rv = _bcm_kt_bfd_l2x_entry_clear(unit, 1,
                                         endpoint_info->label,
                                         endpoint_info->local_discr);
#else
        return BCM_E_UNAVAIL;
#endif /* BCM_TRIUMPH_SUPPORT && BCM_MPLS_SUPPORT */
        break;

    default:
        break;
    }

    return rv;
}
#endif /* BCM_CMICM_SUPPORT */

/*
 * Function:
 *      bcm_kt_bfd_init
 * Purpose:
 *      Initialize the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_MEMORY Unable to allocate memory for internal control structures
 *      BCM_E_INTERNAL Failed to initialize
 * Notes:
 *      BFD initialization will return BCM_E_NONE for the following
 *      error conditions (i.e. feature not supported):
 *      - uKernel is not loaded
 *      - BFD application is not found in any of the uC uKernel
 */
int
bcm_kt_bfd_init(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    int rv = BCM_E_NONE;
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    shr_bfd_msg_ctrl_init_t msg_init;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int uc;
    int priority;

    /* uKernel is not loaded */
    /* TODO: if uKernel is not ready silently return for now */
    if(SOC_E_NONE != soc_cmic_uc_msg_active_wait(unit, 0))
    {
        soc_cm_debug(DK_WARN, "uKernel not ready, BFD not started\n");
        return BCM_E_NONE;
    }

    FEATURE_CHECK(unit);

    if (bfd_info->initialized)
    {
        BCM_IF_ERROR_RETURN(bcm_kt_bfd_detach(unit));
    } else {
        bfd_info->cosq_spath_index      = -1;
        bfd_info->cosq_ach_err_index    = -1;
    }

    /*
     * Initialize HOST side
     */

    bfd_info->unit = unit;

    /* Get SOC properties */
    bfd_info->endpoint_count =
        soc_property_get(unit, spn_BFD_NUM_SESSIONS, 256);
    if (bfd_info->endpoint_count > _BFD_MAX_NUM_ENDPOINTS) {
        bfd_info->endpoint_count = 0;
        return BCM_E_CONFIG;
    }
    bfd_info->encap_size =
        soc_property_get(unit, spn_BFD_ENCAP_MEMORY_SIZE, 1024);
    bfd_info->num_auth_sha1_keys =
        soc_property_get(unit, spn_BFD_SHA1_KEYS, 0);
    bfd_info->num_auth_sp_keys =
        soc_property_get(unit, spn_BFD_SIMPLE_PASSWORD_KEYS, 0);
    bfd_info->cpu_cosq =
        soc_property_get(unit, spn_BFD_COSQ, BFD_COSQ_INVALID);


    /* Allocate memory for endpoints */
    bfd_info->endpoints =
        _bcm_kt_bfd_alloc_clear(bfd_info->endpoint_count *
                                sizeof(_endpoint_config_t),
                                "BFD endpoints");
    bfd_info->endpoints_in_use =
        _bcm_kt_bfd_alloc_clear(SHR_BITALLOCSIZE(bfd_info->endpoint_count),
                                "BFD endpoints in use");

    if ((bfd_info->endpoints == NULL) ||
        (bfd_info->endpoints_in_use == NULL)) {
        _bcm_kt_bfd_free_memory(unit, bfd_info);
        return BCM_E_MEMORY;
    }

    /* Allocate memory for authentications */
    if (bfd_info->num_auth_sha1_keys > 0) {
        bfd_info->auth_sha1 =
            _bcm_kt_bfd_alloc_clear(bfd_info->num_auth_sha1_keys *
                                    sizeof(bcm_bfd_auth_sha1_t),
                                    "BFD auth sha1");
        if (bfd_info->auth_sha1 == NULL) {
            _bcm_kt_bfd_free_memory(unit, bfd_info);
            return BCM_E_MEMORY;
        }
    }
    if (bfd_info->num_auth_sp_keys > 0) {
        bfd_info->auth_sp =
            _bcm_kt_bfd_alloc_clear(bfd_info->num_auth_sp_keys *
                                    sizeof(bcm_bfd_auth_simple_password_t),
                                    "BFD auth simple password");
        if (bfd_info->auth_sp == NULL) {
            _bcm_kt_bfd_free_memory(unit, bfd_info);
            return BCM_E_MEMORY;
        }
    }

    /*
     * Allocate DMA buffers
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    bfd_info->dma_buffer_len = sizeof(shr_bfd_msg_ctrl_t);
    bfd_info->dma_buffer = soc_cm_salloc(unit, bfd_info->dma_buffer_len,
                                         "BFD DMA buffer");
    if (!bfd_info->dma_buffer) {
        _bcm_kt_bfd_free_memory(unit, bfd_info);
        return BCM_E_MEMORY;
    }
    sal_memset(bfd_info->dma_buffer, 0, bfd_info->dma_buffer_len);


    /*
     * Initialize uController side
     */

    /*
     * Start BFD application in BTE (Broadcom Task Engine) uController.
     * Determine which uController is running BFD  by choosing the first
     * uC that returns successfully.
     */
    for (uc = 0; uc < CMICM_NUM_UCS; uc++) {
        rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_BFD,
                                   _BFD_UC_MSG_TIMEOUT_USECS,
                                   BFD_SDK_VERSION,
                                   BFD_UC_MIN_VERSION);
        if (SOC_E_NONE == rv) {
            /* BFD started successfully */
            bfd_info->uc_num = uc;
            break;
        }
    }

    if (uc >= CMICM_NUM_UCS) {
        /* Could not find or start BFD appl */
        _bcm_kt_bfd_free_memory(unit, bfd_info);
        soc_cm_debug(DK_WARN, "uKernel BFD application not available\n");
        return BCM_E_NONE;
    }

    /* RX DMA channel (0..3) local to the uC */
    bfd_info->rx_channel = BCM_KT_BFD_RX_CHANNEL;


    /* Set control message data */
    sal_memset(&msg_init, 0, sizeof(msg_init));
    msg_init.num_sessions       = bfd_info->endpoint_count;
    msg_init.encap_size         = bfd_info->encap_size;
    msg_init.num_auth_sha1_keys = bfd_info->num_auth_sha1_keys;
    msg_init.num_auth_sp_keys   = bfd_info->num_auth_sp_keys;
    msg_init.rx_channel         = bfd_info->rx_channel;
    
    /* Pack control message data into DMA buffer */
    buffer     = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_init_pack(buffer, &msg_init);
    buffer_len = buffer_ptr - buffer;

    /* Send BFD Init message to uC */
    rv = _bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_INIT,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BFD_INIT_REPLY,
                                      &reply_len);

    if (BCM_FAILURE(rv) || (reply_len != 0)) {
        _bcm_kt_bfd_free_memory(unit, bfd_info);
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_INTERNAL;
        }
        return rv;
    }

    /*
     * Initialize HW
     */
    rv = _bcm_kt_bfd_hw_init(unit);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(bcm_kt_bfd_detach(unit));
        return rv;
    }


#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit))
    {
        _bcm_kt_bfd_warm_boot(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */


    /*
     * Initialize Event handler
     */
    /* Start event thread handler */
#if 0
    priority = soc_property_get(unit,
                                spn_BCM_BFD_THREAD_PRI,
                                BFD_THREAD_PRI_DFLT);
#else
    priority = BFD_THREAD_PRI_DFLT;
#endif

    if (bfd_info->event_thread_id == NULL) {
        if (sal_thread_create("bcmBFD", SAL_THREAD_STKSZ, 
                              priority,
                              _bcm_kt_bfd_callback_thread,
                              (void*)bfd_info) == SAL_THREAD_ERROR) {
            return BCM_E_MEMORY;
        }
    }
    bfd_info->event_thread_kill = 0;

    /* Module has been initialized */
    bfd_info->initialized = 1;

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_detach
 * Purpose:
 *      Shut down the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 *      In progress...
 */
int
bcm_kt_bfd_detach(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    int rv;
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _endpoint_config_t *endpoint_config;
    uint16 reply_len;
    int index;

    FEATURE_CHECK(unit);

    if (!bfd_info->initialized) {
        return BCM_E_NONE;
    }

    /* Delete BFD Sessions in HW device */
    for (index = 0; index < bfd_info->endpoint_count; index++) {
        if (SHR_BITGET(bfd_info->endpoints_in_use, index)) {
            endpoint_config = BFD_ENDPOINT_CONFIG(unit, index);

            _bcm_kt_bfd_session_hw_delete(unit, endpoint_config);

            SHR_BITCLR(bfd_info->endpoints_in_use,
                       endpoint_config->endpoint_index);
            endpoint_config->endpoint_index = -1;
        }
    }

    /* Event Handler thread exit signal */
    bfd_info->event_thread_kill = 1;
    soc_cmic_uc_msg_receive_cancel(unit, bfd_info->uc_num,
                                   MOS_MSG_CLASS_BFD_EVENT);

    /*
     * Send BFD Uninit message to uC
     * Ignore error since that may indicate uKernel was reloaded.
     */
    rv = _bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_UNINIT,
                                      0, 0,
                                      MOS_MSG_SUBCLASS_BFD_UNINIT_REPLY,
                                      &reply_len);
    if (BCM_SUCCESS(rv) && (reply_len != 0)) {
        return BCM_E_INTERNAL;
    }

    /* Delete CPU COS queue mapping entries for BFD packets */
    if (bfd_info->cosq_spath_index >= 0) {
        (void) bcm_esw_rx_cosq_mapping_delete(unit,
                                              bfd_info->cosq_spath_index);
        bfd_info->cosq_spath_index = -1;
    }
    if (bfd_info->cosq_ach_err_index >= 0) {
        (void) bcm_esw_rx_cosq_mapping_delete(unit,
                                              bfd_info->cosq_ach_err_index);
        bfd_info->cosq_ach_err_index = -1;
    }

    /* Delete entry for special Session-ID for Unknown Remote Discriminator */
    (void) _bcm_kt_bfd_l2x_entry_clear(unit,
                                       0, 0,
                                       SHR_BFD_DISCR_UNKNOWN_YOUR_DISCR);

    /* Free resources */
    _bcm_kt_bfd_event_unregister_all(bfd_info);
    _bcm_kt_bfd_free_memory(unit, bfd_info);

    bfd_info->initialized = 0;

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_endpoint_create
 * Purpose:
 *      Create or update an BFD endpoint object
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_PARAM     Invalid parameter specified
 *      BCM_E_NOT_FOUND Attempt to update an endpoint which does not exist
 *      BCM_E_EXISTS    Attempt to create an endpoint with a specified ID
 *                      which is already in use
 *      BCM_E_FULL      No free endpoints available.
 *
 * Notes:
 */
int
bcm_kt_bfd_endpoint_create(int unit,
                           bcm_bfd_endpoint_info_t *endpoint_info)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
     _endpoint_config_t *endpoint_config;
    int update;
    int endpoint_index;
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    int local_id;
    bcm_port_t tx_port;
    int is_local;
    bcm_l3_egress_t l3_egress;
    shr_bfd_msg_ctrl_sess_set_t msg_sess;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int encap = 0;
    uint32 session_flags;
    _mpls_label_t mpls_label;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(endpoint_info);

    /* Check Authentication key index */
    if (endpoint_info->auth == bcmBFDAuthTypeNone) {
        /* Nothing to check */
    } else if (endpoint_info->auth == bcmBFDAuthTypeSimplePassword) {
        AUTH_SP_INDEX_CHECK(unit, (int)endpoint_info->auth_index);
    } else if ((endpoint_info->auth == bcmBFDAuthTypeKeyedSHA1) ||
               (endpoint_info->auth == bcmBFDAuthTypeMeticulousKeyedSHA1)) {
        AUTH_SHA1_INDEX_CHECK(unit, (int)endpoint_info->auth_index);
    } else {
        /*
         * Not supported:
         * bcmBFDAuthTypeKeyedMD5 and bcmBFDAuthTypeMeticulousKeyedMD5
         */
        return BCM_E_PARAM;
    }

    /* Resolve module and port */
    sal_memset(&l3_egress, 0, sizeof(l3_egress));
    if (BCM_GPORT_INVALID != endpoint_info->gport) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, endpoint_info->gport,
                                    &module_id, &port_id,
                                    &trunk_id, &local_id));
        if (trunk_id != BCM_TRUNK_INVALID) {
            return BCM_E_PARAM;
        }
    } else if (BCM_SUCCESS
               (bcm_esw_l3_egress_get(unit, endpoint_info->egress_if,
                                      &l3_egress))) {
        module_id = l3_egress.module;
        port_id = l3_egress.port;
    } else {
        return BCM_E_PARAM;
    }

    /* Get local port used for TX BFD packet */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_modid_is_local(unit, module_id, &is_local));
    if (is_local) {    /* Ethernet port */
        tx_port = port_id;
    } else {           /* HG port */
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modport_get(unit, module_id, &tx_port));
    }


    /* Create or Update */
    update = (endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE) ? 1 : 0;
    session_flags = (update) ? 0 : SHR_BFD_SESS_SET_F_CREATE;

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_WITH_ID) {
        endpoint_index = endpoint_info->id;

        ENDPOINT_INDEX_CHECK(unit, endpoint_index);

        /*
         * Update requires endpoint ID to exist
         * Create requires endpoint ID not to exist
         */
        if (update &&
            (!SHR_BITGET(bfd_info->endpoints_in_use, endpoint_index))) {
            return BCM_E_NOT_FOUND;
        } else if (!update &&
                   (SHR_BITGET(bfd_info->endpoints_in_use, endpoint_index))) {
            return BCM_E_EXISTS;
        }

    } else {    /* No ID */

        if (update) {    /* Update specified with no ID */
            return BCM_E_PARAM;
        }

        /* Allocate endpoint */
        endpoint_index =
            _bcm_kt_bfd_find_free_endpoint(bfd_info->endpoints_in_use,
                                           bfd_info->endpoint_count);
        if (endpoint_index < 0) {
            return BCM_E_FULL;
        }

        endpoint_info->id = endpoint_index;
    }

    /* Get Endpoint config entry */
    endpoint_config = BFD_ENDPOINT_CONFIG(unit, endpoint_index);

    /* Endpoint tunnel type cannot change */
    if (update && (endpoint_config->info.type != endpoint_info->type)) {
        return BCM_E_PARAM;
    }

    /*
     * Compare new and old BFD parameters for Update
     */
    if (update) {
        if (endpoint_config->info.local_discr !=
            endpoint_info->local_discr) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DISC;
        }
        if (endpoint_config->info.local_min_tx !=
            endpoint_info->local_min_tx) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_MIN_TX;
        }
        if (endpoint_config->info.local_min_rx !=
            endpoint_info->local_min_rx) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_MIN_RX;
        }
        if (endpoint_config->info.local_min_echo !=
            endpoint_info->local_min_echo) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_MIN_ECHO_RX;
        }
        if (endpoint_config->info.local_diag !=
            endpoint_info->local_diag) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DIAG;
        }
        if ((endpoint_config->info.flags & BCM_BFD_ENDPOINT_DEMAND)
            != (endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND)) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DEMAND;
        }
        if (endpoint_config->info.local_detect_mult !=
            endpoint_info->local_detect_mult) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DETECT_MULT;
        }
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_ENCAP_SET) {
            session_flags |= SHR_BFD_SESS_SET_F_ENCAP;
        }
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_SHA1_SEQUENCE_INCR) {
            session_flags |= SHR_BFD_SESS_SET_F_SHA1_XMT_SEQ_INCR;
        }
    }

    /* Set Endpoint config entry */
    endpoint_config->endpoint_index = endpoint_index;
    endpoint_config->modid          = module_id;
    endpoint_config->port           = port_id;
    endpoint_config->tx_port        = tx_port;
    endpoint_config->info           = *endpoint_info;

    /* Set BFD Session in HW */
    if (!update || (endpoint_info->flags & BCM_BFD_ENDPOINT_ENCAP_SET)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_session_hw_set(unit, endpoint_config, module_id,
                                        port_id, is_local));
        encap = 1;
    }

    /* Set control message data */
    sal_memset(&msg_sess, 0, sizeof(msg_sess));

    /*
     * Set the BFD encapsulation data
     *
     * The function _bcm_kt_bfd_encap_create() is called first
     * since this sets some fields in 'endpoint_config' which are
     * used in the message.
     */
    if (encap) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_encap_create(unit, endpoint_config,
                                      msg_sess.encap_data));
    }
    msg_sess.sess_id = endpoint_config->endpoint_index;
    msg_sess.flags   = session_flags;
    msg_sess.passive =
        (endpoint_info->flags & BCM_BFD_ENDPOINT_PASSIVE) ? 1 : 0;
    msg_sess.local_demand =
        (endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND) ? 1 : 0;
    msg_sess.local_diag           = endpoint_info->local_diag;
    msg_sess.local_detect_mult    = endpoint_info->local_detect_mult;
    msg_sess.local_discriminator  = endpoint_info->local_discr;
    msg_sess.remote_discriminator = 0;
    msg_sess.local_min_tx         = endpoint_info->local_min_tx;
    msg_sess.local_min_rx         = endpoint_info->local_min_rx;
    msg_sess.local_min_echo_rx    = endpoint_info->local_min_echo;
    msg_sess.auth_type            = endpoint_info->auth;
    msg_sess.auth_key             = endpoint_info->auth_index;
    msg_sess.xmt_auth_seq         = endpoint_info->tx_auth_seq;
    if (encap) {
        msg_sess.encap_type   = endpoint_config->encap_type;
        msg_sess.encap_length = endpoint_config->encap_length;
        msg_sess.lkey_etype   = endpoint_config->lkey_etype;
        msg_sess.lkey_offset  = endpoint_config->lkey_offset;
        msg_sess.lkey_length  = endpoint_config->lkey_length;
        if (endpoint_info->mep_id_length) {
            msg_sess.mep_id_length = endpoint_info->mep_id_length;
            sal_memcpy(msg_sess.mep_id, endpoint_info->mep_id,
                       endpoint_info->mep_id_length);
        }
    }
    /* MPLS label needs to be in packet frame format (label field only) */
    _bcm_kt_bfd_mpls_label_get(endpoint_info->label, 0, 0, 0,
                               &mpls_label);
    _bcm_kt_bfd_mpls_label_pack(msg_sess.mpls_label, &mpls_label);

    msg_sess.tx_port = endpoint_config->tx_port;
    msg_sess.tx_cos  = endpoint_info->int_pri;
    msg_sess.tx_pri  = 0;
    msg_sess.tx_qnum = (0 == msg_sess.tx_port) ?
        0 : ((8 * msg_sess.tx_port) + 40);


    /* Pack control message data into DMA buffer */
    buffer     = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_sess_set_pack(buffer, &msg_sess);
    buffer_len = buffer_ptr - buffer;

    /* Send BFD Session Set message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_SESS_SET,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BFD_SESS_SET_REPLY,
                                      &reply_len));
    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }


    /* Mark endpoint in use */
    SHR_BITSET(bfd_info->endpoints_in_use, endpoint_config->endpoint_index);

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_endpoint_get
 * Purpose:
 *      Get an BFD endpoint object
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint      - (IN) The ID of the endpoint object to get.
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure
 *                            to receive the data.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND Endpoint does not exist
 *      BCM_E_PARAM     Invalid parameter specified
 *      BCM_E_INTERNAL  Failed to read endpoint information
 * Notes:
 */
int
bcm_kt_bfd_endpoint_get(int unit,
                        bcm_bfd_endpoint_t endpoint,
                        bcm_bfd_endpoint_info_t *endpoint_info)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _endpoint_config_t *endpoint_config;
    shr_bfd_msg_ctrl_sess_get_t msg_sess;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(endpoint_info);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!SHR_BITGET(bfd_info->endpoints_in_use, endpoint)) {
        return BCM_E_NOT_FOUND;
    }

    endpoint_config = BFD_ENDPOINT_CONFIG(unit, endpoint);
    sal_memcpy(endpoint_info, &endpoint_config->info,
               sizeof(*endpoint_info));

    /* Send BFD Session Get message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_SESS_GET,
                                      endpoint, 0,
                                      MOS_MSG_SUBCLASS_BFD_SESS_GET_REPLY,
                                      &reply_len));

    /* Unpack control message data from DMA buffer */
    sal_memset(&msg_sess, 0, sizeof(msg_sess));
    buffer     = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_sess_get_unpack(buffer, &msg_sess);
    buffer_len = buffer_ptr - buffer;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }
    
    endpoint_info->local_state        = msg_sess.local_sess_state;
    endpoint_info->local_discr        = msg_sess.local_discriminator;
    endpoint_info->local_diag         = msg_sess.local_diag;
    endpoint_info->tx_auth_seq        = msg_sess.xmt_auth_seq;
    endpoint_info->rx_auth_seq        = msg_sess.rcv_auth_seq;
    endpoint_info->remote_state       = msg_sess.remote_sess_state;
    endpoint_info->remote_discr       = msg_sess.remote_discriminator;
    endpoint_info->remote_diag        = msg_sess.remote_diag;
    endpoint_info->remote_min_tx      = msg_sess.remote_min_tx;
    endpoint_info->remote_min_rx      = msg_sess.remote_min_rx;
    endpoint_info->remote_min_echo    = msg_sess.remote_min_echo_rx;
    endpoint_info->remote_detect_mult = msg_sess.remote_detect_mult;

    endpoint_info->remote_flags = 0;
    if (msg_sess.remote_demand) {
        endpoint_info->remote_flags |= BCM_BFD_ENDPOINT_REMOTE_DEMAND;
    }

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_endpoint_destroy
 * Purpose:
 *      Destroy an BFD endpoint object.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to destroy.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to destroy endpoint which does not exist
 *      BCM_E_INTERNAL  Unable to release resource /
 *                      Failed to read memory or read or write register
 * Notes:
 */
int
bcm_kt_bfd_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _endpoint_config_t *endpoint_config;
    uint16 reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!SHR_BITGET(bfd_info->endpoints_in_use, endpoint)) {
        return BCM_E_NOT_FOUND;
    }

    endpoint_config = BFD_ENDPOINT_CONFIG(unit, endpoint);

    /* Delete BFD Session in HW */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_session_hw_delete(unit, endpoint_config));

    /* Send BFD Session Delete message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_SESS_DELETE,
                                      endpoint_config->endpoint_index, 0,
                                      MOS_MSG_SUBCLASS_BFD_SESS_DELETE_REPLY,
                                      &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }


    SHR_BITCLR(bfd_info->endpoints_in_use, endpoint_config->endpoint_index);
    endpoint_config->endpoint_index = -1;

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_endpoint_destroy_all
 * Purpose:
 *      Destroy all BFD endpoint objects.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_kt_bfd_endpoint_destroy_all(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _endpoint_config_t *endpoint_config;
    uint16 reply_len;
    int index;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    for (index = 0; index < bfd_info->endpoint_count; index++) {
        if (SHR_BITGET(bfd_info->endpoints_in_use, index)) {
            endpoint_config = BFD_ENDPOINT_CONFIG(unit, index);

            _bcm_kt_bfd_session_hw_delete(unit, endpoint_config);

            SHR_BITCLR(bfd_info->endpoints_in_use,
                       endpoint_config->endpoint_index);
            endpoint_config->endpoint_index = -1;
        }
    }

    /* Send BFD Session Delete All message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive
         (unit,
          MOS_MSG_SUBCLASS_BFD_SESS_DELETE_ALL,
          0, 0,
          MOS_MSG_SUBCLASS_BFD_SESS_DELETE_ALL_REPLY,
          &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND BFD Session ID not in use
 *      BCM_E_INTERNAL  Unable to read or write resource
 *      BCM_E_PARAM     Session not in Demand Mode/
 *                      Session not in UP state
 * Notes: The poll operation will be initiated.  Poll failure will be
 * signalled via asynchronous callback (aka session failure)
 */
int
bcm_kt_bfd_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _endpoint_config_t *endpoint_config;
    uint16 reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!SHR_BITGET(bfd_info->endpoints_in_use, endpoint)) {
        return BCM_E_NOT_FOUND;
    }

    endpoint_config = BFD_ENDPOINT_CONFIG(unit, endpoint);

    /* Check for Demand mode */
    if (!(endpoint_config->info.flags & BCM_BFD_ENDPOINT_DEMAND)) {
        return BCM_E_CONFIG;
    }

    /* Issue poll request on session not in the UP state will fail */

    /* Send BFD Session Poll message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_SESS_POLL,
                                      endpoint_config->endpoint_index, 0,
                                      MOS_MSG_SUBCLASS_BFD_SESS_POLL_REPLY,
                                      &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_kt_bfd_event_register(int unit,
                          bcm_bfd_event_types_t event_types,
                          bcm_bfd_event_cb cb, void *user_data)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    uint32 result;
    _event_handler_t *event_handler;
    _event_handler_t *previous = NULL;
    bcm_bfd_event_type_t event_type;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(cb);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0) {
        return BCM_E_PARAM;
    }

    for (event_handler = bfd_info->event_handler_list;
         event_handler != NULL;
         event_handler = event_handler->next) {
        if (event_handler->cb == cb) {
            break;
        }

        previous = event_handler;
    }

    if (event_handler == NULL) {
        /* This handler hasn't been registered yet */

        event_handler = sal_alloc(sizeof(*event_handler), "BFD event handler");

        if (event_handler == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(event_handler, 0, sizeof(*event_handler));

        event_handler->next = NULL;
        event_handler->cb = cb;

        SHR_BITCLR_RANGE(event_handler->event_types.w, 0, bcmBFDEventCount);

        if (previous != NULL) {
            previous->next = event_handler;
        } else {
            bfd_info->event_handler_list = event_handler;
        }
    }

    for (event_type = 0; event_type < bcmBFDEventCount; ++event_type) {
        if (SHR_BITGET(event_types.w, event_type)) {
            if (!SHR_BITGET(event_handler->event_types.w, event_type)) {
                /* This handler isn't handling this event yet */
                SHR_BITSET(event_handler->event_types.w, event_type);
            }
        }
    }

    event_handler->user_data = user_data;

    /* Update Events mask */
    return _bcm_kt_bfd_event_mask_set(unit);

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_kt_bfd_event_unregister(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    uint32 result;
    _event_handler_t *event_handler;
    _event_handler_t *previous = NULL;
    bcm_bfd_event_type_t event_type;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(cb);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0) {
        return BCM_E_PARAM;
    }

    for (event_handler = bfd_info->event_handler_list;
         event_handler != NULL;
         event_handler = event_handler->next) {
        if (event_handler->cb == cb) {
            break;
        }

        previous = event_handler;
    }

    if (event_handler == NULL) {
        return BCM_E_NOT_FOUND;
    }

    for (event_type = 0; event_type < bcmBFDEventCount; ++event_type) {
        if (SHR_BITGET(event_types.w, event_type)) {
            SHR_BITCLR(event_handler->event_types.w, event_type);
        }
    }

    SHR_BITTEST_RANGE(event_handler->event_types.w, 0, bcmBFDEventCount,
        result);

    if (result == 0) {
        /* No more events for this handler to handle */
        if (previous != NULL) {
            previous->next = event_handler->next;
        } else {
            bfd_info->event_handler_list = event_handler->next;
        }

        sal_free(event_handler);
    }

    /* Update Events mask */
    return _bcm_kt_bfd_event_mask_set(unit);

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to counter information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_kt_bfd_endpoint_stat_get(int unit,
                             bcm_bfd_endpoint_t endpoint,
                             bcm_bfd_endpoint_stat_t *ctr_info, uint8 clear)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    shr_bfd_msg_ctrl_stat_req_t stat_req;
    shr_bfd_msg_ctrl_stat_reply_t stat_reply;
    uint8 *buffer_req, *buffer_reply, *buffer_ptr;
    uint16 buffer_len, reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(ctr_info);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!SHR_BITGET(bfd_info->endpoints_in_use, endpoint)) {
        return BCM_E_NOT_FOUND;
    }

    /* Set control message data */
    sal_memset(&stat_req, 0, sizeof(stat_req));
    stat_req.sess_id = endpoint;
    stat_req.clear   = clear;

    /* Pack control message data into DMA */
    buffer_req = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_stat_req_pack(buffer_req, &stat_req);
    buffer_len = buffer_ptr - buffer_req;

    /* Send BFD Stat Get message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_STAT_GET,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BFD_STAT_GET_REPLY,
                                      &reply_len));

    /* Unpack control message data from DMA buffer */
    sal_memset(&stat_reply, 0, sizeof(stat_reply));
    buffer_reply = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_stat_reply_unpack(buffer_reply, &stat_reply);
    buffer_len = buffer_ptr - buffer_reply;
    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }

    ctr_info->packets_in = stat_reply.packets_in;
    ctr_info->packets_out = stat_reply.packets_out;
    ctr_info->packets_drop = stat_reply.packets_drop;
    ctr_info->packets_auth_drop = stat_reply.packets_auth_drop;

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_kt_bfd_auth_sha1_set(int unit,
                         int index, bcm_bfd_auth_sha1_t *sha1)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    shr_bfd_msg_ctrl_auth_sha1_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(sha1);

    AUTH_SHA1_INDEX_CHECK(unit, index);

    /* Set control message data */
    sal_memset(&msg_auth, 0, sizeof(msg_auth));
    msg_auth.index    = index;
    msg_auth.enable   = sha1->enable;
    msg_auth.sequence = sha1->sequence;
    sal_memcpy(msg_auth.key, sha1->key, _SHR_BFD_AUTH_SHA1_KEY_LENGTH);

    /* Pack control message data into DMA buffer */
    buffer     = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_auth_sha1_pack(buffer, &msg_auth);
    buffer_len = buffer_ptr - buffer;

    /* Send BFD Auth Sha1 Set message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET_REPLY,
                                      &reply_len));
    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    bfd_info->auth_sha1[index] = *sha1;

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


/*
 * Function:
 *      bcm_kt_bfd_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_kt_bfd_auth_sha1_get(int unit,
                         int index, bcm_bfd_auth_sha1_t *sha1)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    shr_bfd_msg_ctrl_auth_sha1_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(sha1);

    AUTH_SHA1_INDEX_CHECK(unit, index);

    /* If warmboot, get authentication data from uController, else use cache */
    if (SOC_WARM_BOOT(unit)) {

        /* Send BFD Auth Sha1 Get message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_GET,
              index, 0,
              MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_GET_REPLY,
              &reply_len));

        /* Unpack control message data from DMA buffer */
        sal_memset(&msg_auth, 0, sizeof(msg_auth));
        buffer     = bfd_info->dma_buffer;
        buffer_ptr = shr_bfd_msg_ctrl_auth_sha1_unpack(buffer, &msg_auth);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            return BCM_E_INTERNAL;
        }

        bfd_info->auth_sha1[index].enable   = msg_auth.enable;
        bfd_info->auth_sha1[index].sequence = msg_auth.sequence;
        sal_memcpy(bfd_info->auth_sha1[index].key, msg_auth.key,
                   _SHR_BFD_AUTH_SHA1_KEY_LENGTH);

    }

    *sha1 = bfd_info->auth_sha1[index];

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_kt_bfd_auth_simple_password_set(int unit,
                                    int index,
                                    bcm_bfd_auth_simple_password_t *sp)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    shr_bfd_msg_ctrl_auth_sp_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(sp);

    AUTH_SP_INDEX_CHECK(unit, index);

    /* Set control message data */
    sal_memset(&msg_auth, 0, sizeof(msg_auth));
    msg_auth.index  = index;
    msg_auth.length = sp->length;
    sal_memcpy(msg_auth.password, sp->password,
               _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH);

    /* Pack control message data into DMA buffer */
    buffer     = bfd_info->dma_buffer;
    buffer_ptr = shr_bfd_msg_ctrl_auth_sp_pack(buffer, &msg_auth);
    buffer_len = buffer_ptr - buffer;

    /* Send BFD Auth SimplePassword Set message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_bfd_msg_send_receive(unit,
                                      MOS_MSG_SUBCLASS_BFD_AUTH_SP_SET,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BFD_AUTH_SP_SET_REPLY,
                                      &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    bfd_info->auth_sp[index] = *sp;

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_kt_bfd_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_kt_bfd_auth_simple_password_get(int unit,
                                    int index,
                                    bcm_bfd_auth_simple_password_t *sp)
{
#ifdef BCM_CMICM_SUPPORT
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    shr_bfd_msg_ctrl_auth_sp_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;

    FEATURE_CHECK(unit);

    INIT_CHECK(unit);

    PARAM_NULL_CHECK(sp);

    AUTH_SP_INDEX_CHECK(unit, index);

    /* If warmboot, get authentication data from uController, else use cache */
    if (SOC_WARM_BOOT(unit)) {

        /* Send BFD Auth SimplePassword Get message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_bfd_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_AUTH_SP_GET,
              index, 0,
              MOS_MSG_SUBCLASS_BFD_AUTH_SP_GET_REPLY,
              &reply_len));

        /* Unpack control message data from DMA buffer */
        sal_memset(&msg_auth, 0, sizeof(msg_auth));
        buffer     = bfd_info->dma_buffer;
        buffer_ptr = shr_bfd_msg_ctrl_auth_sp_unpack(buffer, &msg_auth);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            return BCM_E_INTERNAL;
        }

        bfd_info->auth_sp[index].length = msg_auth.length;
        sal_memcpy(bfd_info->auth_sp[index].password, msg_auth.password,
                   _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH);
    }

    *sp = bfd_info->auth_sp[index];

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *      _bcm_kt_bfd_sw_dump
 * Purpose:
 *      Displays BFD information maintained by software.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      None
 */
void
_bcm_kt_bfd_sw_dump(int unit)
{
    _bfd_info_t *bfd_info = BFD_INFO(unit);
    _endpoint_config_t *endpoint_config;
    int endpoint_idx;
    SHR_BITDCL word;

    soc_cm_print("\nSW Information BFD - Unit %d\n", unit);

    soc_cm_print("\n endpoints_in_use \n");
    for (word = 0; word < _SHR_BITDCLSIZE
             (bfd_info->endpoint_count); word++) {
        soc_cm_print(" word %d value %x ", word,
                     bfd_info->endpoints_in_use[word]);
    }

    soc_cm_print("\n Endpoint Information \n");
    for (endpoint_idx = 0; endpoint_idx < bfd_info->endpoint_count;
         endpoint_idx++) {
        if (!SHR_BITGET(bfd_info->endpoints_in_use, endpoint_idx)) {
            continue;
        }
        endpoint_config = BFD_ENDPOINT_CONFIG(unit, endpoint_idx);
        soc_cm_print("\n Endpoint index %d\n", endpoint_idx);
        soc_cm_print("\t Type %x\n", endpoint_config->info.type);
        soc_cm_print("\t VRF id %x\n", endpoint_config->info.vrf_id);
    }

    return;
}
#endif


#else /* BCM_KATANA_SUPPORT && INCLUDE_BFD */
int _bcm_esw_kt_bfd_not_empty;
#endif /* BCM_KATANA_SUPPORT && INCLUDE_BFD */
