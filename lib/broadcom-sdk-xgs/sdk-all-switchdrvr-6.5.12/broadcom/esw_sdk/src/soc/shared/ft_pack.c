/*
 * $Id: ft_pack.c
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        ft_pack.c
 * Purpose:     FT pack and unpack routines for:
 *              - FT Control messages
 *
 *
 * FT control messages
 *
 * FT messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The FT control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/ft.h>
#include <soc/shared/ft_msg.h>
#include <soc/shared/ft_pack.h>

/*
 * FT Initialization control message packing
 */
uint8*
shr_ft_msg_ctrl_init_pack(uint8 *buf, shr_ft_msg_ctrl_init_t *msg)
{
    int i, j;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->max_groups);

    _SHR_PACK_U8(buf, msg->num_pipes);

    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_PACK_U16(buf, msg->pipe[i].num_ports);

        for (j = 0; j < msg->pipe[i].num_ports; j++) {
            _SHR_PACK_U16(buf, msg->pipe[i].ports[j]);
        }

        _SHR_PACK_U32(buf, msg->pipe[i].max_flows);

        _SHR_PACK_U8(buf, msg->pipe[i].num_ctr_pools);
        for (j = 0; j < msg->pipe[i].num_ctr_pools; j++) {
            _SHR_PACK_U8(buf, msg->pipe[i].ctr_pools[j]);
            _SHR_PACK_U16(buf, msg->pipe[i].ctr_pool_size[j]);
        }
    }

    _SHR_PACK_U32(buf, msg->rx_channel);
    _SHR_PACK_U16(buf, msg->max_export_pkt_length);
    _SHR_PACK_U32(buf, msg->cur_time_secs);

    _SHR_PACK_U32(buf, msg->export_interval_msecs);
    _SHR_PACK_U32(buf, msg->scan_interval_msecs);
    _SHR_PACK_U32(buf, msg->age_timer_tick_msecs);

    _SHR_PACK_U8(buf, msg->num_elph_profiles);

    return buf;
}

/*
 * FT Initialization control message
 */
uint8*
shr_ft_msg_ctrl_init_unpack(uint8 *buf, shr_ft_msg_ctrl_init_t *msg)
{
    int i, j;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->max_groups);

    _SHR_UNPACK_U8(buf, msg->num_pipes);

    for (i = 0; i < msg->num_pipes; i++) {
        _SHR_UNPACK_U16(buf, msg->pipe[i].num_ports);

        for (j = 0; j < msg->pipe[i].num_ports; j++) {
            _SHR_UNPACK_U16(buf, msg->pipe[i].ports[j]);
        }

        _SHR_UNPACK_U32(buf, msg->pipe[i].max_flows);

        _SHR_UNPACK_U8(buf, msg->pipe[i].num_ctr_pools);
        for (j = 0; j < msg->pipe[i].num_ctr_pools; j++) {
            _SHR_UNPACK_U8(buf, msg->pipe[i].ctr_pools[j]);
            _SHR_UNPACK_U16(buf, msg->pipe[i].ctr_pool_size[j]);
        }
    }

    _SHR_UNPACK_U32(buf, msg->rx_channel);
    _SHR_UNPACK_U16(buf, msg->max_export_pkt_length);
    _SHR_UNPACK_U32(buf, msg->cur_time_secs);

    _SHR_UNPACK_U32(buf, msg->export_interval_msecs);
    _SHR_UNPACK_U32(buf, msg->scan_interval_msecs);
    _SHR_UNPACK_U32(buf, msg->age_timer_tick_msecs);

    _SHR_UNPACK_U8(buf, msg->num_elph_profiles);

    return buf;
}

/*
 * EM key format export pack
 */
uint8*
shr_ft_msg_ctrl_em_key_format_pack(uint8 *buf,
                                   shr_ft_msg_ctrl_em_key_format_t *msg)
{
    int i, j;


    _SHR_PACK_U16(buf, msg->key_size);
    _SHR_PACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_PACK_U8(buf, msg->qual[i]);
        _SHR_PACK_U8(buf, msg->qual_parts[i].num_parts);

        for (j = 0; j < msg->qual_parts[i].num_parts; j++) {
            _SHR_PACK_U16(buf, msg->qual_parts[i].offset[j]);
            _SHR_PACK_U8(buf, msg->qual_parts[i].width[j]);
        }
    }

    return buf;
}

/*
 * EM key format export unpack
 */
uint8*
shr_ft_msg_ctrl_em_key_format_unpack(uint8 *buf,
                                     shr_ft_msg_ctrl_em_key_format_t *msg)
{
    int i, j;


    _SHR_UNPACK_U16(buf, msg->key_size);
    _SHR_UNPACK_U8(buf, msg->num_qual);
    for (i = 0; i < msg->num_qual; i++) {
        _SHR_UNPACK_U8(buf, msg->qual[i]);
        _SHR_UNPACK_U8(buf, msg->qual_parts[i].num_parts);

        for (j = 0; j < msg->qual_parts[i].num_parts; j++) {
            _SHR_UNPACK_U16(buf, msg->qual_parts[i].offset[j]);
            _SHR_UNPACK_U8(buf, msg->qual_parts[i].width[j]);
        }
    }

    return buf;
}

/*
 * Group create pack
 */
uint8*
shr_ft_msg_ctrl_group_create_pack(uint8 *buf,
                                  shr_ft_msg_ctrl_group_create_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->flow_limit);
    _SHR_PACK_U32(buf, msg->aging_interval_msecs);
    _SHR_PACK_U32(buf, msg->domain_id);

    return buf;
}

/*
 * Group create unpack
 */
uint8*
shr_ft_msg_ctrl_group_create_unpack(uint8 *buf,
                                    shr_ft_msg_ctrl_group_create_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->flow_limit);
    _SHR_UNPACK_U32(buf, msg->aging_interval_msecs);
    _SHR_UNPACK_U32(buf, msg->domain_id);

    return buf;
}

/*
 * Group get pack
 */
uint8*
shr_ft_msg_ctrl_group_get_pack(uint8* buf, shr_ft_msg_ctrl_group_get_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->flow_limit);
    _SHR_PACK_U32(buf, msg->flow_count);
    _SHR_PACK_U32(buf, msg->domain_id);
    _SHR_PACK_U32(buf, msg->aging_interval);
    _SHR_PACK_U16(buf, msg->elph_profile_id);
    _SHR_PACK_U16(buf, msg->qos_profile_id);
    _SHR_PACK_U16(buf, msg->collector_id);
    _SHR_PACK_U16(buf, msg->template_id);

    return buf;
}

/*
 * Group get unpack
 */
uint8*
shr_ft_msg_ctrl_group_get_unpack(uint8* buf, shr_ft_msg_ctrl_group_get_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->flow_limit);
    _SHR_UNPACK_U32(buf, msg->flow_count);
    _SHR_UNPACK_U32(buf, msg->domain_id);
    _SHR_UNPACK_U32(buf, msg->aging_interval);
    _SHR_UNPACK_U16(buf, msg->elph_profile_id);
    _SHR_UNPACK_U16(buf, msg->qos_profile_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);
    _SHR_UNPACK_U16(buf, msg->template_id);

    return buf;
}


/*
 * Group update pack
 */
uint8*
shr_ft_msg_ctrl_group_update_pack(uint8* buf,
                                  shr_ft_msg_ctrl_group_update_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->update);
    _SHR_PACK_U32(buf, msg->param0);

    return buf;
}

/*
 * Group update unpack
 */
uint8*
shr_ft_msg_ctrl_group_update_unpack(uint8* buf,
                                    shr_ft_msg_ctrl_group_update_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->update);
    _SHR_UNPACK_U32(buf, msg->param0);

    return buf;
}

/*
 * Create IPFIX template - pack
 */
uint8*
shr_ft_msg_ctrl_template_create_pack(uint8* buf, shr_ft_msg_ctrl_template_create_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->set_id);

    _SHR_PACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_PACK_U8(buf, msg->info_elems[i]);
        _SHR_PACK_U8(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Create IPFIX template - unpack
 */
uint8*
shr_ft_msg_ctrl_template_create_unpack(uint8* buf, shr_ft_msg_ctrl_template_create_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->set_id);

    _SHR_UNPACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_UNPACK_U8(buf, msg->info_elems[i]);
        _SHR_UNPACK_U8(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Get IPFIX template - pack
 */
uint8*
shr_ft_msg_ctrl_template_get_pack(uint8* buf, shr_ft_msg_ctrl_template_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->set_id);

    _SHR_PACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_PACK_U8(buf, msg->info_elems[i]);
        _SHR_PACK_U8(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Get IPFIX template - unpack
 */
uint8*
shr_ft_msg_ctrl_template_get_unpack(uint8* buf, shr_ft_msg_ctrl_template_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->set_id);

    _SHR_UNPACK_U8(buf, msg->num_info_elems);
    for (i = 0; i < msg->num_info_elems; i++) {
        _SHR_UNPACK_U8(buf, msg->info_elems[i]);
        _SHR_UNPACK_U8(buf, msg->data_size[i]);
    }

    return buf;
}

/*
 * Create IPFIX collector - pack
 */
uint8*
shr_ft_msg_ctrl_collector_create_pack(uint8* buf, shr_ft_msg_ctrl_collector_create_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    _SHR_PACK_U16(buf, msg->mtu);
    _SHR_PACK_U32(buf, msg->ip_base_checksum);
    _SHR_PACK_U32(buf, msg->udp_base_checksum);
    _SHR_PACK_U16(buf, msg->ip_offset);
    _SHR_PACK_U16(buf, msg->udp_offset);

    return buf;
}

/*
 * Create IPFIX collector - unpack
 */
uint8*
shr_ft_msg_ctrl_collector_create_unpack(uint8* buf, shr_ft_msg_ctrl_collector_create_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    _SHR_UNPACK_U16(buf, msg->mtu);
    _SHR_UNPACK_U32(buf, msg->ip_base_checksum);
    _SHR_UNPACK_U32(buf, msg->udp_base_checksum);
    _SHR_UNPACK_U16(buf, msg->ip_offset);
    _SHR_UNPACK_U16(buf, msg->udp_offset);

    return buf;
}

/*
 * Get IPFIX collector - pack
 */
uint8*
shr_ft_msg_ctrl_collector_get_pack(uint8* buf, shr_ft_msg_ctrl_collector_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    _SHR_PACK_U16(buf, msg->mtu);
    _SHR_PACK_U16(buf, msg->ip_base_checksum);
    _SHR_PACK_U16(buf, msg->udp_base_checksum);
    _SHR_PACK_U16(buf, msg->ip_offset);
    _SHR_PACK_U16(buf, msg->udp_offset);

    return buf;
}

/*
 * Get IPFIX collector - unpack
 */
uint8*
shr_ft_msg_ctrl_collector_get_unpack(uint8* buf, shr_ft_msg_ctrl_collector_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->id);
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    _SHR_UNPACK_U16(buf, msg->mtu);
    _SHR_UNPACK_U16(buf, msg->ip_base_checksum);
    _SHR_UNPACK_U16(buf, msg->udp_base_checksum);
    _SHR_UNPACK_U16(buf, msg->ip_offset);
    _SHR_UNPACK_U16(buf, msg->udp_offset);

    return buf;
}

/*
 * Get data associated with a flow - pack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_pack(uint8* buf,
                                         shr_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    _SHR_PACK_U16(buf, msg->group_idx);
    _SHR_PACK_U32(buf, msg->src_ip);
    _SHR_PACK_U32(buf, msg->dst_ip);
    _SHR_PACK_U16(buf, msg->l4_src_port);
    _SHR_PACK_U16(buf, msg->l4_dst_port);
    _SHR_PACK_U8(buf, msg->ip_protocol);

    return buf;
}

/*
 * Get data associated with a flow - unpack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_unpack(uint8* buf,
                                          shr_ft_msg_ctrl_group_flow_data_get_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->group_idx);
    _SHR_UNPACK_U32(buf, msg->src_ip);
    _SHR_UNPACK_U32(buf, msg->dst_ip);
    _SHR_UNPACK_U16(buf, msg->l4_src_port);
    _SHR_UNPACK_U16(buf, msg->l4_dst_port);
    _SHR_UNPACK_U8(buf, msg->ip_protocol);

    return buf;
}

/*
 * Data associated with a flow - pack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_reply_pack(
                                    uint8* buf,
                                    shr_ft_msg_ctrl_group_flow_data_get_reply_t *msg)
{
    _SHR_PACK_U32(buf, msg->pkt_count_upper);
    _SHR_PACK_U32(buf, msg->pkt_count_lower);
    _SHR_PACK_U32(buf, msg->byte_count_upper);
    _SHR_PACK_U32(buf, msg->byte_count_lower);

    return buf;
}

/*
 * Data associated with a flow - unpack
 */
uint8*
shr_ft_msg_ctrl_group_flow_data_get_reply_unpack(
                                    uint8* buf,
                                    shr_ft_msg_ctrl_group_flow_data_get_reply_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->pkt_count_upper);
    _SHR_UNPACK_U32(buf, msg->pkt_count_lower);
    _SHR_UNPACK_U32(buf, msg->byte_count_upper);
    _SHR_UNPACK_U32(buf, msg->byte_count_lower);

    return buf;
}

/*
 * SER event - pack
 */
uint8*
shr_ft_msg_ctrl_ser_event_pack(uint8* buf, shr_ft_msg_ctrl_ser_event_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->pipe);
    _SHR_PACK_U32(buf, msg->mem);
    _SHR_PACK_U32(buf, msg->index);

    return buf;
}

/*
 * SER event - unpack
 */
uint8*
shr_ft_msg_ctrl_ser_event_unpack(uint8* buf, shr_ft_msg_ctrl_ser_event_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U8(buf, msg->pipe);
    _SHR_UNPACK_U32(buf, msg->mem);
    _SHR_UNPACK_U32(buf, msg->index);

    return buf;
}


/*
 * Template set transmit - pack
 */
uint8*
shr_ft_msg_ctrl_template_xmit_pack(uint8* buf,
                                   shr_ft_msg_ctrl_template_xmit_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->template_id);
    _SHR_PACK_U16(buf, msg->collector_id);
    _SHR_PACK_U16(buf, msg->initial_burst);
    _SHR_PACK_U16(buf, msg->interval_secs);
    _SHR_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    return buf;
}

/*
 * Template set transmit - unpack
 */
uint8*
shr_ft_msg_ctrl_template_xmit_unpack(uint8* buf,
                                     shr_ft_msg_ctrl_template_xmit_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U16(buf, msg->template_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);
    _SHR_UNPACK_U16(buf, msg->initial_burst);
    _SHR_UNPACK_U16(buf, msg->interval_secs);
    _SHR_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    return buf;
}

/*
 * Add elephant profile - pack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_create_pack(uint8* buf,
                                         shr_ft_msg_ctrl_elph_profile_create_t *msg)
{
    int i;

    _SHR_PACK_U8(buf, msg->profile_idx);
    _SHR_PACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_PACK_U16(buf, msg->filters[i].scan_count);
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_PACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_PACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

/*
 * Add elephant profile - unpack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_create_unpack(uint8* buf,
                                           shr_ft_msg_ctrl_elph_profile_create_t *msg)
{
    int i;

    _SHR_UNPACK_U8(buf, msg->profile_idx);
    _SHR_UNPACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_UNPACK_U16(buf, msg->filters[i].scan_count);
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

/*
 * Get elephant profile - pack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_get_pack(uint8* buf,
                                      shr_ft_msg_ctrl_elph_profile_get_t *msg)
{
    int i;

    _SHR_PACK_U8(buf, msg->profile_idx);
    _SHR_PACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_PACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_PACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_PACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}

/*
 * Get elephant profile - unpack
 */
uint8*
shr_ft_msg_ctrl_elph_profile_get_unpack(uint8* buf,
                                        shr_ft_msg_ctrl_elph_profile_get_t *msg)
{
    int i;

    _SHR_UNPACK_U8(buf, msg->profile_idx);
    _SHR_UNPACK_U8(buf, msg->num_filters);

    for (i = 0; i <  msg->num_filters; i++) {
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_upper_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].size_threshold_bytes_lower_32);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_high_threshold);
        _SHR_UNPACK_U32(buf, msg->filters[i].rate_low_threshold);
    }
    return buf;
}
