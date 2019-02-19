/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    shr_ft_msg.h
 */

#ifndef SHR_FT_MSG_H_
#define SHR_FT_MSG_H_

#include <soc/shared/ft.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif


/*
 * Per pipe info used for FT Initialization control message
 */
typedef struct __COMPILER_ATTR__ _ft_msg_ctrl_init_pipe_s {
    /* Max flows */
    uint32 max_flows;

    /* Num ports in the pipe */
    uint16 num_ports;

    /* List of ports */
    uint16 ports[SHR_FT_MAX_PIPE_PORTS];

    /* Size of reserved pools */
    uint16 ctr_pool_size[SHR_FT_MAX_CTR_POOLS];

    /* Reserved Flex counter pools per pipe */
    uint8 ctr_pools[SHR_FT_MAX_CTR_POOLS];

    /* Number of reserved counter pools */
    uint8 num_ctr_pools;

} shr_ft_msg_ctrl_init_pipe_t;

/*
 * FT Initialization control message
 */

/* Enable flow start timestamp storing */
#define SHR_FT_MSG_INIT_FLAGS_FLOW_START_TS 0x1

/* Enable Elephant monitoring */
#define SHR_FT_MSG_INIT_FLAGS_ELEPHANT 0x2

typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_init_s {
    /* Initialization flags */
    uint32 flags;

    /* Max number of groups */
    uint32 max_groups;

    /* Local RX DMA channel */
    uint32 rx_channel;

    /* Current time in seconds */
    uint32 cur_time_secs;

    /* Export interval in msec */
    uint32 export_interval_msecs;

    /* Scan interval in msec */
    uint32 scan_interval_msecs;

    /* Aging timer tick in msec */
    uint32 age_timer_tick_msecs;

    /* Per H/w pipe info */
    shr_ft_msg_ctrl_init_pipe_t pipe[SHR_FT_MAX_PIPES];

    /* Maximum length of the transmitted packet */
    uint16 max_export_pkt_length;

    /* Num ports in the device */
    uint8 num_pipes;

    /* Number of elephant profiles */
    uint8 num_elph_profiles;
} shr_ft_msg_ctrl_init_t;

/*
 * EM key format export
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_em_qual_s {
    /* Offsets into the key */
    uint16 offset[SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /* Width of qualifiers */
    uint8  width[SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /* Num offsets of the qualifier */
    uint8  num_parts;
} shr_ft_msg_ctrl_qual_parts_t;

typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_em_key_format_s {
    /* Offsets and width of each qual */
    shr_ft_msg_ctrl_qual_parts_t qual_parts[SHR_FT_EM_NUM_QUAL];

    /* Size of the key in bits */
    uint16 key_size;

    /* Qualifiers */
    uint8 qual[SHR_FT_EM_NUM_QUAL];

    /* Number of qualifiers */
    uint8 num_qual;
} shr_ft_msg_ctrl_em_key_format_t;

/*
 * FT group create
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_create_s {
    /* Flags */
    uint32  flags;

    /* Max flows that can be learned on this group */
    uint32  flow_limit;

    /* Aging interval in msecs */
    uint32 aging_interval_msecs;

    /* IPFIX domain id */
    uint32 domain_id;

    /* Group Index */
    uint16  group_idx;
} shr_ft_msg_ctrl_group_create_t;

/*
 * FT group get
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_get_s {
    /* Flags */
    uint32 flags;

    /* Max flows that can be learned on this group */
    uint32 flow_limit;

    /* Num flows that learnt on this group */
    uint32 flow_count;

    /* Aging interval in msecs */
    uint32 aging_interval;

    /* IPFIX domain id */
    uint32 domain_id;

    /* Group Index */
    uint16 group_idx;

    /* Elephant profile Id */
    uint16 elph_profile_id;

    /* QoS profile Id */
    uint16 qos_profile_id;

    /* Collector Id */
    uint16 collector_id;

    /* Template Id */
    uint16 template_id;

} shr_ft_msg_ctrl_group_get_t;

/*
 * FT group update
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_update_s {
    /* Flags */
    uint32  flags;

    /* Indicates what is being updated */
    uint32  update;

    /* Param value depending on the update */
    uint32  param0;

    /* Group Index */
    uint16 group_idx;
} shr_ft_msg_ctrl_group_update_t;

/*
 * Create IPFIX template
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_template_create_s {
    /* Flags */
    uint32 flags;

    /* Software handle for template */
    uint16 id;

    /* IPFIX set id */
    uint16 set_id;

    /* Number of information elements */
    uint8 num_info_elems;

    /* Information elements in the order they are to be exported */
    uint8 info_elems[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];

    /* Size in bytes of the info elems */
    uint8 data_size[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} shr_ft_msg_ctrl_template_create_t;

/*
 * Get IPFIX template
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_template_get_s {
    /* Flags */
    uint32 flags;

    /* Software handle for template */
    uint16 id;

    /* IPFIX set id */
    uint16 set_id;

    /* Number of information elements */
    uint8 num_info_elems;

    /* Information elements in the order they are to be exported */
    uint8 info_elems[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];

    /* Size in bytes of the info elems */
    uint8 data_size[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} shr_ft_msg_ctrl_template_get_t;

/*
 * Create IPFIX collector
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_collector_create_s {
    /* Flags */
    uint32 flags;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Software handle for collector */
    uint16 id;

    /* Length of the IPFIX encapsulation */
    uint16 encap_length;

    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;

    /* Collector encapsulation */
    uint8 encap[SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH];
} shr_ft_msg_ctrl_collector_create_t;

/*
 * Get IPFIX collector
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_collector_get_s {
    /* Flags */
    uint32 flags;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Software handle for collector */
    uint16 id;

    /* Length of the IPFIX encapsulation */
    uint16 encap_length;

    /* Collector encapsulation */
    uint16 encap[SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH];

    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;
} shr_ft_msg_ctrl_collector_get_t;

/*
 * Get data associated with a flow
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_flow_data_get_s {
    /* Source IP */
    uint32 src_ip;

    /* Destination IP */
    uint32 dst_ip;

    /* Group Index */
    uint16 group_idx;

    /* L4 source port */
    uint16 l4_src_port;

    /* L4 destination port */
    uint16 l4_dst_port;

    /* IP protocol */
    uint8  ip_protocol;
} shr_ft_msg_ctrl_group_flow_data_get_t;

/*
 * Data associated with a flow
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_flow_data_get_reply_s {
    /* Upper 32 bits of the packet counter */
    uint32 pkt_count_upper;

    /* Lower 32 bits of the packet counter */
    uint32 pkt_count_lower;

    /* Upper 32 bits of the byte counter */
    uint32 byte_count_upper;

    /* Lower 32 bits of the byte counter */
    uint32 byte_count_lower;
} shr_ft_msg_ctrl_group_flow_data_get_reply_t;

/*
 * FT SER event
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_ser_event_s {
    /* Flags */
    uint32 flags;

    /* Memory */
    uint32 mem;

    /* Index */
    uint32 index;

    /* Pipe */
    uint8 pipe;
} shr_ft_msg_ctrl_ser_event_t;

/*
 * Start the transmission of the template set
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_template_xmit_s {
    /* Flags */
    uint32 flags;

    /* Template id */
    uint16 template_id;

    /* Collector id */
    uint16 collector_id;

    /* Initial burst of packets to be sent out */
    uint16 initial_burst;

    /* Transmit Interval */
    uint16 interval_secs;

    /* Length of the encap */
    uint16 encap_length;

    /* Template encap */
    uint8 encap[SHR_FT_MAX_TEMPLATE_SET_ENCAP_LENGTH];

} shr_ft_msg_ctrl_template_xmit_t;

/*
 * Elephant profile filter
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_elph_profile_filter_s {
    /* Upper 32 bits of the size threshold in bytes */
    uint32 size_threshold_bytes_upper_32;

    /* Upper 32 bits of the size threshold in bytes */
    uint32 size_threshold_bytes_lower_32;

    /* Low rate threshold in bytes/msec */
    uint32 rate_low_threshold;

    /* High rate threshold in bytes/msec */
    uint32 rate_high_threshold;

    /* Scan count */
    uint16 scan_count;

    /* 1 - Rate has to increase every scan */
    uint8 incr_rate;
} shr_ft_msg_ctrl_elph_profile_filter_t;

/*
 * Add an elephant profile
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_elph_profile_create_s {
    /* Filters arranged in order, 2 Promotions and 1 Demotion */
    shr_ft_msg_ctrl_elph_profile_filter_t filters[SHR_FT_ELPH_MAX_FILTERS];

    /* Profile Id */
    uint8 profile_idx;

    /* Number of filters, added for completeness sake, must be always 3 */
    uint8 num_filters;
} shr_ft_msg_ctrl_elph_profile_create_t;

/*
 * Get an elephant profile
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_elph_profile_get_s {
    /* Filters arranged in order, 2 Promotions and 1 Demotion */
    shr_ft_msg_ctrl_elph_profile_filter_t filters[SHR_FT_ELPH_MAX_FILTERS];

    /* Profile Id */
    uint8 profile_idx;

    /* Number of filters, added for completeness sake, must be always 3 */
    uint8 num_filters;
} shr_ft_msg_ctrl_elph_profile_get_t;

/*
 *  FT control messages
 */
typedef union shr_ft_msg_ctrl_s {
    shr_ft_msg_ctrl_init_t                      init;
    shr_ft_msg_ctrl_em_key_format_t             em_key_format;
    shr_ft_msg_ctrl_group_create_t              group_create;
    shr_ft_msg_ctrl_group_get_t                 group_get;
    shr_ft_msg_ctrl_group_update_t              group_update;
    shr_ft_msg_ctrl_template_create_t           template_create;
    shr_ft_msg_ctrl_template_get_t              template_get;
    shr_ft_msg_ctrl_collector_create_t          collector_create;
    shr_ft_msg_ctrl_collector_get_t             collector_get;
    shr_ft_msg_ctrl_group_flow_data_get_t       data_get;
    shr_ft_msg_ctrl_group_flow_data_get_reply_t data_get_reply;
    shr_ft_msg_ctrl_ser_event_t                 ser_event;
    shr_ft_msg_ctrl_template_xmit_t             tmpl_xmit;
    shr_ft_msg_ctrl_elph_profile_create_t       eprofile_create;
    shr_ft_msg_ctrl_elph_profile_get_t          eprofile_get;
} shr_ft_msg_ctrl_t;


#endif /* SHR_FT_MSG_H_ */
