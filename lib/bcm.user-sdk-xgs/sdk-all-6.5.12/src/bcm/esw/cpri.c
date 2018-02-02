#include <bcm/cpri.h>
#ifdef CPRIMOD_SUPPORT
#include <soc/cprimod/cprimod.h>
#endif /* CPRIMOD_SUPPORT */
#include <bcm/error.h>
#include <bcm/debug.h>
#include <shared/bsl.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>

int _bcm_esw_port_gport_validate(int unit, bcm_port_t port_in,
                                 bcm_port_t *port_out);

#endif /* PORTMOD_SUPPORT */


#ifdef CPRIMOD_SUPPORT
static bcm_cpri_port_speed_t
_int_to_bcm_cpri_port_speed_t (
    int speed_i)
{
    bcm_cpri_port_speed_t speed;

    switch(speed_i) {
    case 1228:
        speed = bcmCpriPortSpeed1228p8;
        break;
    case 2457:
        speed = bcmCpriPortSpeed2457p6;
        break;
    case 3072:
        speed = bcmCpriPortSpeed3072p0;
        break;
    case 4915:
        speed = bcmCpriPortSpeed4915p2;
        break;
    case 6144:
        speed = bcmCpriPortSpeed6144p0;
        break;
    case 9830:
        speed = bcmCpriPortSpeed9830p4;
        break;
    case 10137:
        speed = bcmCpriPortSpeed10137p6;
        break;
    case 12165:
        speed = bcmCpriPortSpeed12165p12;
        break;
    case 24330:
        speed = bcmCpriPortSpeed24330p24;
        break;
    default:
        speed = bcmCpriPortSpeed1228p8;
        break;
    }

    return speed;
}

static int
_bcm_cpri_port_speed_t_to_int (
    bcm_cpri_port_speed_t speed)
{
    int speed_i;

    switch(speed) {
    case bcmCpriPortSpeed1228p8:
        speed_i = 1228;
        break;
    case bcmCpriPortSpeed2457p6:
        speed_i = 2457;
        break;
    case bcmCpriPortSpeed3072p0:
        speed_i = 3072;
        break;
    case bcmCpriPortSpeed4915p2:
        speed_i = 4915;
        break;
    case bcmCpriPortSpeed6144p0:
        speed_i = 6144;
        break;
    case bcmCpriPortSpeed9830p4:
        speed_i = 9830;
        break;
    case bcmCpriPortSpeed10137p6:
        speed_i = 10137;
        break;
    case bcmCpriPortSpeed12165p12:
        speed_i = 12165;
        break;
    case bcmCpriPortSpeed24330p24:
        speed_i = 24330;
        break;
    default:
        speed_i = 1228;
        break;
    }

    return speed_i;
}


static int
_portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(
    portmod_cpri_axc_frame_info_t *axc_info_t,
    bcm_cpri_axc_frame_info_t *axc_info)
{
    axc_info->start_bit = axc_info_t->start_bit;
    axc_info->num_bits  = axc_info_t->num_bits;
    axc_info->map_method  = axc_info_t->container_info.map_method;
    if (axc_info_t->container_info.map_method == cprimod_cpri_frame_map_method_1) {
        axc_info->stuffing_count = axc_info_t->container_info.Nst;
    } else {
        axc_info->stuffing_count = axc_info_t->container_info.Nv;
    }

    axc_info->K = axc_info_t->container_info.K;
    axc_info->sample_size_M = axc_info_t->encap_data_info.sample_size;
    axc_info->Nv = axc_info_t->container_info.Nv;
    axc_info->Na = axc_info_t->container_info.Na;
    axc_info->Nc = axc_info_t->container_info.Nc;
    axc_info->truncation_enable = axc_info_t->encap_data_info.truncation_enable;
    switch (axc_info_t->container_info.frame_sync_mode) {
    case cprimod_cpri_frame_sync_mode_hyper:
        axc_info->frame_sync_mode = bcmCpriFrameSyncModeHyper;
        break;
    case cprimod_cpri_frame_sync_mode_radio:
        axc_info->frame_sync_mode = bcmCpriFrameSyncModeRadio;
        break;
    case cprimod_cpri_frame_sync_mode_basic:
        axc_info->frame_sync_mode = bcmCpriFrameSyncModeBasic;
        break;
    default:
        axc_info->frame_sync_mode = bcmCpriFrameSyncModeBasic;
        break;
    }
    axc_info->basic_frame_offset = axc_info_t->container_info.basic_frame_offset;
    axc_info->radio_frame_offset = axc_info_t->container_info.radio_frame_offset;
    axc_info->hyper_frame_offset = axc_info_t->container_info.hyper_frame_offset;
    axc_info->num_samples_per_roe_pkt = axc_info_t->roe_payload_info.packet_size/(2*axc_info_t->encap_data_info.sample_size);
    switch (axc_info_t->encap_data_info.truncation_type) {
    case cprimod_truncation_type_15_to_16:
        axc_info->truncation_mode = bcmCpriFrameTruncMode15_to_16;
        break;
    case cprimod_truncation_type_add_0:
        axc_info->truncation_mode = bcmCpriFrameTruncModeAdd_0;
        break;
    case cprimod_truncation_type_16_to_15:
        axc_info->truncation_mode = bcmCpriFrameTruncMode16_to_15;
        break;
    case cprimod_truncation_type_add_1:
        axc_info->truncation_mode = bcmCpriFrameTruncModeAdd_1;
        break;
    default:
        axc_info->truncation_mode = bcmCpriFrameTruncMode15_to_16;
        break;
    }
    switch (axc_info_t->encap_data_info.compression_type) {
    case cprimod_compress:
        axc_info->compress_mode = bcmCpriFrameCompressModeCompress;
        break;
    case cprimod_decompress:
        axc_info->compress_mode = bcmCpriFrameCompressModeDecompress;
        break;
    case cprimod_no_compress:
        axc_info->compress_mode = bcmCpriFrameCompressModeNone;
        break;
    default:
        axc_info->compress_mode = bcmCpriFrameCompressModeCompress;
        break;
    }
    axc_info->priority = axc_info_t->encap_data_info.priority;
    axc_info->rx_queue_size = axc_info_t->encap_data_info.buffer_size;
    axc_info->cycle_size = axc_info_t->encap_data_info.cycle_size;

    return BCM_E_NONE;
}

static int
_bcm_cpri_axc_frame_info_t_to_portmod_cpri_axc_frame_info_t(
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info,
    portmod_cpri_axc_frame_info_t *axc_info_t)
{
    axc_info_t->start_bit = axc_info->start_bit;
    axc_info_t->num_bits  = axc_info->num_bits;
    axc_info_t->container_info.map_method  = axc_info->map_method;
    axc_info_t->container_info.Nst = axc_info->stuffing_count;
    axc_info_t->container_info.K = axc_info->K;
    axc_info_t->encap_data_info.sample_size = axc_info->sample_size_M;
    if (axc_info->map_method == 0) {
        axc_info_t->container_info.Naxc = axc_info->num_bits;
    } else {
        axc_info_t->container_info.Naxc = 2*axc_info->sample_size_M;
    }
    axc_info_t->container_info.Nv = axc_info->Nv;
    axc_info_t->container_info.Na = axc_info->Na;
    axc_info_t->container_info.Nc = axc_info->Nc;
    switch (axc_info->frame_sync_mode) {
    case bcmCpriFrameSyncModeHyper:
        axc_info_t->container_info.frame_sync_mode = cprimod_cpri_frame_sync_mode_hyper;
        break;
    case bcmCpriFrameSyncModeRadio:
        axc_info_t->container_info.frame_sync_mode = cprimod_cpri_frame_sync_mode_radio;
        break;
    case bcmCpriFrameSyncModeBasic:
        axc_info_t->container_info.frame_sync_mode = cprimod_cpri_frame_sync_mode_basic;
        break;
    default:
        axc_info_t->container_info.frame_sync_mode = cprimod_cpri_frame_sync_mode_basic;
        break;
    }
    axc_info_t->container_info.basic_frame_offset = axc_info->basic_frame_offset;
    axc_info_t->container_info.radio_frame_offset = axc_info->radio_frame_offset;
    axc_info_t->container_info.hyper_frame_offset = axc_info->hyper_frame_offset;
    axc_info_t->roe_payload_info.packet_size = axc_info->num_samples_per_roe_pkt * (2*axc_info->sample_size_M);
    /* For now no last packet */
    axc_info_t->roe_payload_info.last_packet_num = 0;
    axc_info_t->roe_payload_info.last_packet_size = axc_info->num_samples_per_roe_pkt * (2*axc_info->sample_size_M);
    axc_info_t->encap_data_info.truncation_enable = axc_info->truncation_enable;
    switch (axc_info->truncation_mode) {
    case bcmCpriFrameTruncMode15_to_16:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_15_to_16;
        break;
    case bcmCpriFrameTruncModeAdd_0:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_add_0;
        break;
    case bcmCpriFrameTruncMode16_to_15:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_16_to_15;
        break;
    case bcmCpriFrameTruncModeAdd_1:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_add_1;
        break;
    default:
        axc_info_t->encap_data_info.truncation_type = cprimod_truncation_type_15_to_16;
        break;
    }
    switch (axc_info->compress_mode) {
    case bcmCpriFrameCompressModeCompress:
        axc_info_t->encap_data_info.compression_type = cprimod_compress;
        break;
    case bcmCpriFrameCompressModeDecompress:
        axc_info_t->encap_data_info.compression_type = cprimod_decompress;
        break;
    case bcmCpriFrameCompressModeNone:
        axc_info_t->encap_data_info.compression_type = cprimod_no_compress;
        break;
    default:
        axc_info_t->encap_data_info.compression_type = cprimod_compress;
        break;
    }
    if (axc_id < 64) {
        axc_info_t->encap_data_info.mux_enable = 1;
    } else {
        axc_info_t->encap_data_info.mux_enable = 0;
    }
    axc_info_t->encap_data_info.bit_reversal = 0;
    axc_info_t->encap_data_info.priority = axc_info->priority;
    axc_info_t->encap_data_info.buffer_size = axc_info->rx_queue_size;
    axc_info_t->encap_data_info.cycle_size = axc_info->cycle_size;

    return BCM_E_NONE;
}
#endif

int
bcm_esw_cpri_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_axc_agnostic_mode_enable_set(
    int unit,
    bcm_gport_t port,
    int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_axc_agnostic_mode_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_agnostic_config_t *config)
{
    return BCM_E_UNAVAIL;
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_bcm_to_cprimod_ethertype(bcm_cpri_ethertype_t bcm_type, cprimod_ethertype_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriEthertypeRoe:
            *cprimod_type = cprimodEthertypeRoe;
            break;

        case bcmCpriEthertypeFast:
            *cprimod_type = cprimodEthertypeFast;
            break;

        case bcmCpriEthertypeVlan:
            *cprimod_type = cprimodEthertypeVlan;
            break;

        case bcmCpriEthertypeQinQ:
            *cprimod_type = cprimodEthertypeQinQ;
            break;

        default:
            break;
    }
    return SOC_E_NONE;
}
#endif

int
bcm_esw_cpri_port_decap_ethertype_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_ethertype_t type,
    uint16 *tpid)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_ethertype_t int_type=0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    _bcm_bcm_to_cprimod_ethertype(type, &int_type);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_ethertype_config_get(unit, port, int_type, tpid));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_ethertype_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_ethertype_t type,
    uint16 tpid)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_ethertype_t int_type=0;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    _bcm_bcm_to_cprimod_ethertype(type, &int_type);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_ethertype_config_set(unit, port, int_type, tpid));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_config_clear(
    int unit,
    bcm_gport_t port,
    uint8 roe_subtype)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_decap_flow_config_get(
    int unit,
    bcm_gport_t port,
    uint8 roe_subtype,
    bcm_cpri_decap_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cls_option_t option;
    cprimod_cls_flow_type_t flow_type;
    uint32 queue_num;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_config_get(unit, port, roe_subtype, &queue_num, &option, &flow_type));

    switch (option) {
        case cprimodClsOptionUseQueueNum:
            config->lookup_option = bcmCpriDecapFlowLookupQueueNum;
            break;
        case cprimodClsOptionUseFlowIdToQueue:
            config->lookup_option = bcmCpriDecapFlowLookupFlowID;
            break;
        case cprimodClsOptionUseOpcodeToQueue:
            config->lookup_option = bcmCpriDecapFlowLookupOpcode;
            break;
        default:
            config->lookup_option = bcmCpriDecapFlowLookupQueueNum;
        break;
    }

    switch (flow_type) {
        case cprimodClsFlowTypeData:
            config->flow_type = bcmCpriDecapFlowTypeIQData;
            break;
        case cprimodClsFlowTypeDataWithExt:
            config->flow_type = bcmCpriDecapFlowTypeIQDataWithTimestamp;
            break;
        case cprimodClsFlowTypeCtrlWithOpcode: 
            config->flow_type = bcmCpriDecapFlowTypeIQDataWithOpcode;
            break;
        case cprimodClsFlowTypeCtrl:
            config->flow_type = bcmCpriDecapFlowTypeControl;
            break;
        default:
            config->flow_type = bcmCpriDecapFlowTypeIQData;
            break;
    }
    config->queue_num = queue_num;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_config_set(
    int unit,
    bcm_gport_t port,
    uint8 roe_subtype,
    bcm_cpri_decap_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cls_option_t option;
    cprimod_cls_flow_type_t flow_type;
    uint32 queue_num;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (config->lookup_option) {
        case bcmCpriDecapFlowLookupQueueNum:
            option = cprimodClsOptionUseQueueNum;
            break;
        case bcmCpriDecapFlowLookupFlowID:
            option = cprimodClsOptionUseFlowIdToQueue;
            break;
        case bcmCpriDecapFlowLookupOpcode:
            option = cprimodClsOptionUseOpcodeToQueue;
            break;
        default:
            option = cprimodClsOptionUseQueueNum;
            break;
    }

    switch (config->flow_type) {
        case bcmCpriDecapFlowTypeIQData:
            flow_type = cprimodClsFlowTypeData;
            break;
        case bcmCpriDecapFlowTypeIQDataWithTimestamp:
            flow_type = cprimodClsFlowTypeDataWithExt;
            break;
        case bcmCpriDecapFlowTypeIQDataWithOpcode: 
            flow_type = cprimodClsFlowTypeCtrlWithOpcode;
            break;
        case bcmCpriDecapFlowTypeControl:
            flow_type = cprimodClsFlowTypeCtrl;
            break;
        default:
            flow_type = cprimodClsFlowTypeData;
            break;

    }
    queue_num =  config->queue_num;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_config_set(unit, port, roe_subtype, queue_num, option, flow_type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_to_queue_mapping_set(
    int unit,
    bcm_gport_t port,
    uint32 flow_id,
    uint32 queue_num)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_set(unit, port, flow_id, queue_num));
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_flow_to_queue_mapping_get(
    int unit,
    bcm_gport_t port,
    uint32 flow_id,
    uint32 *queue_num)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_flow_to_queue_mapping_get(unit, port, flow_id, queue_num));
    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}


int
bcm_esw_cpri_port_decap_queue_to_ordering_info_index_set(
    int unit,
    bcm_gport_t port,
    uint32 queue_num,
    uint32 ordering_info_index)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_queue_to_ordering_info_index_set(unit, port, queue_num, ordering_info_index));

    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_queue_to_ordering_info_index_get(
    int unit,
    bcm_gport_t port,
    uint32 queue_num,
    uint32 *ordering_info_index)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_queue_to_ordering_info_index_get(unit, port, queue_num, ordering_info_index));

    return SOC_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_bcm_to_cprimod_ordering_info_type (bcm_cpri_roe_info_type_t bcm_type, cprimod_ordering_info_type_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriRoEInfoTypeUseSeq:
            *cprimod_type = cprimod_ordering_info_type_roe_sequence;
            break;

        case bcmCpriRoEInfoTypeBfnForQcnt:
            *cprimod_type = cprimod_ordering_info_type_bfn_for_qcnt;
            break;

        case bcmCpriRoEInfoTypePInfo:
            *cprimod_type = cprimod_ordering_info_type_use_pinfo;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}


static
int _bcm_cprimod_to_bcm_ordering_info_type (cprimod_ordering_info_type_t cprimod_type, bcm_cpri_roe_info_type_t *bcm_type)
{
    switch (cprimod_type) {
        case cprimod_ordering_info_type_roe_sequence:
            *bcm_type = bcmCpriRoEInfoTypeUseSeq;
            break;

        case cprimod_ordering_info_type_bfn_for_qcnt:
            *bcm_type = bcmCpriRoEInfoTypeBfnForQcnt;
            break;

        case cprimod_ordering_info_type_use_pinfo:
            *bcm_type = bcmCpriRoEInfoTypePInfo;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}
#endif

int
bcm_esw_cpri_port_decap_roe_ordering_info_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_decap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_decap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_decap_ordering_info_entry_t_init(&entry);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_info_get(unit, port, index, &entry));

    _bcm_cprimod_to_bcm_ordering_info_type(entry.type, &ordering_info->info_type);

    ordering_info->p_ext                = entry.pcnt_extended;
    ordering_info->pcnt_pkt_count       = entry.pcnt_pkt_count;
    ordering_info->pcnt_size            = entry.pcnt_size;
    ordering_info->pcnt_increment       = entry.pcnt_increment;
    ordering_info->pcnt_max             = entry.pcnt_max;
    ordering_info->p_bias               = entry.pcnt_bias;
    ordering_info->p_inc_p2             = entry.pcnt_increment_p2;
    ordering_info->qcnt_size            = entry.qcnt_size;
    ordering_info->qcnt_increment       = entry.qcnt_increment;
    ordering_info->qcnt_max             = entry.qcnt_max;
    ordering_info->q_bias               = entry.qcnt_bias;
    ordering_info->q_inc_p2             = entry.qcnt_increment_p2;
    ordering_info->modulo_2             = entry.modulo_2;
    ordering_info->gsm_tsn_bitmap       = entry.gsm_tsn_bitmap;
    ordering_info->seq_rsvd             = entry.seq_reserve_value;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_roe_ordering_info_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_decap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_decap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_decap_ordering_info_entry_t_init(&entry);

    _bcm_bcm_to_cprimod_ordering_info_type(ordering_info->info_type, &entry.type);
    entry.pcnt_extended         = ordering_info->p_ext;
    entry.pcnt_pkt_count        = ordering_info->pcnt_pkt_count;
    entry.pcnt_size             = ordering_info->pcnt_size;
    entry.pcnt_increment        = ordering_info->pcnt_increment;
    entry.pcnt_max              = ordering_info->pcnt_max;
    entry.pcnt_bias             = ordering_info->p_bias;
    entry.pcnt_increment_p2     = ordering_info->p_inc_p2;
    entry.qcnt_size             = ordering_info->qcnt_size;
    entry.qcnt_increment        = ordering_info->qcnt_increment;
    entry.qcnt_max              = ordering_info->qcnt_max;
    entry.qcnt_bias             = ordering_info->q_bias;
    entry.qcnt_increment_p2     = ordering_info->q_inc_p2;
    entry.modulo_2              = ordering_info->modulo_2;
    entry.gsm_tsn_bitmap        = ordering_info->gsm_tsn_bitmap;
    entry.seq_reserve_value     = ordering_info->seq_rsvd;


    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_info_set(unit, port, index, &entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_roe_ordering_sequence_offset_get(
    int unit,
    bcm_gport_t port,
    int queue,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    /* user need to provide the sizes. */
    offset.p_size   = seq_offset->pcnt_size;
    offset.q_size   = seq_offset->qcnt_size;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_sequence_offset_get(unit, port, queue, &offset));

    seq_offset->pcnt_offset = offset.p_offset;
    seq_offset->qcnt_offset = offset.q_offset;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_decap_roe_ordering_sequence_offset_set(
    int unit,
    bcm_gport_t port,
    int queue,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    offset.p_size   = seq_offset->pcnt_size;
    offset.p_offset = seq_offset->pcnt_offset;
    offset.q_size   = seq_offset->qcnt_size;
    offset.q_offset = seq_offset->qcnt_offset;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_decap_roe_ordering_sequence_offset_set(unit, port, queue, &offset));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_clear(
    int unit,
    bcm_gport_t port,
    int queue_num)
{


    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_encap_clear_all(
    int unit,
    bcm_gport_t port)
{
    return BCM_E_UNAVAIL;
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_cprimod_to_bcm_hdr_vlan_type(cprimod_hdr_vlan_type_t cprimod_type, bcm_cpri_vlan_type_t *bcm_type)
{
    switch (cprimod_type) {
        case cprimodHdrVlanTypeUntagged:
            *bcm_type = bcmCpriVlanTypeNone;
            break;

        case cprimodHdrVlanTypeTaggedVlan0:
            *bcm_type = bcmCpriVlanTypeSingle;
            break;

        case cprimodHdrVlanTypeQinQ:
            *bcm_type = bcmCpriVlanTypeQinQ;
            break;
        case cprimodHdrVlanTypeTaggedVlan1:
            *bcm_type = bcmCpriVlanTypeSingleTable1;
            break;
        default:
            *bcm_type = bcmCpriVlanTypeNone;
        break;
    }
    return SOC_E_NONE;

}
#endif

int
bcm_esw_cpri_port_encap_get(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_cpri_encap_info_t *encap_config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_header_config_t config;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    cprimod_encap_header_config_t_init(&config);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_set(unit, port, queue_num, &config));

    _bcm_cprimod_to_bcm_hdr_vlan_type(config.vlan_type, &encap_config->vlan_type);

    encap_config->hdr_type                  = config.header_type;
    encap_config->roe_flow_id               = config.roe_flow_id ;
    encap_config->subtype                   = config.roe_subtype;
    encap_config->roe_ordering_info_index   = config.ordering_info_index;
    encap_config->macda_index               = config.mac_da_index;
    encap_config->macsa_index               = config.mac_sa_index;
    encap_config->vlan0_idx                 = config.vlan_id_0_index;
    encap_config->vlan1_idx                 = config.vlan_id_1_index;
    encap_config->vlan0_priority            = config.vlan_0_priority;
    encap_config->vlan1_priority            = config.vlan_1_priority;
    encap_config->vlan_ethertype_idx        = config.vlan_eth_type_index;
    encap_config->opcode                    = config.roe_opcode;

    if (config.use_tagid_for_vlan) {
        encap_config->flags |= BCM_CPRI_ENCAP_USE_TAG_ID_FOR_VLAN;
    }

    if (config.use_tagid_for_flowid) {
        encap_config->flags |= BCM_CPRI_ENCAP_USE_TAG_ID_FOR_FLOWID;
    }

    if (config.use_opcode) {
        encap_config->flags |= BCM_CPRI_ENCAP_USE_OPCODE;
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macda_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macda_entry_get(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macda_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macda_entry_set(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macsa_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macsa_entry_get(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_macsa_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_mac_t macaddr)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_macsa_entry_set(unit, port, index, macaddr));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_vlan_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    uint32 index,
    bcm_vlan_t *vlan_id)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_vlan_table_id_t table_id = cprimodVlanTable0;
    uint32 int_vlan_id;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (flags == BCM_CPRI_VLAN_FLAG_VLAN_0) {
        table_id = cprimodVlanTable0;
    } else {
        table_id = cprimodVlanTable1;
    }

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_vlan_entry_get(unit, port, index, table_id, &int_vlan_id));

    *vlan_id = int_vlan_id;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_entry_vlan_set(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    uint32 index,
    bcm_vlan_t vlan_id)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_vlan_table_id_t table_id = cprimodVlanTable0;
    uint32 int_vlan_id;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if (flags == BCM_CPRI_VLAN_FLAG_VLAN_0) {
        table_id = cprimodVlanTable0;
    } else {
        table_id = cprimodVlanTable1;
    }
    int_vlan_id = vlan_id;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_roe_encap_vlan_entry_set(unit, port, index, table_id, int_vlan_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_ordering_info_clear(
    int unit,
    bcm_gport_t port,
    int index)
{
    return BCM_E_UNAVAIL;
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_bcm_to_cprimod_ordering_info_incr_type(bcm_cpri_roe_incr_type_t bcm_type, cprimod_ordering_info_prop_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriRoEIncrTypeNone:
            *cprimod_type = cprimod_ordering_info_prop_no_increment;
            break;

        case bcmCpriRoEIncrTypeConstant:
            *cprimod_type = cprimod_ordering_info_prop_increment_by_constant;
            break;

        case bcmCpriRoEIncrTypePayloadSize:
            *cprimod_type = cprimod_ordering_info_prop_increment_by_payload;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

static
int _bcm_cprimod_to_bcm_ordering_info_incr_type(cprimod_ordering_info_prop_t cprimod_type, bcm_cpri_roe_incr_type_t* bcm_type)
{
    switch (cprimod_type) {
        case cprimod_ordering_info_prop_no_increment:
            *bcm_type = bcmCpriRoEIncrTypeNone;
            break;

        case cprimod_ordering_info_prop_increment_by_constant:
            *bcm_type = bcmCpriRoEIncrTypeConstant;
            break;

        case cprimod_ordering_info_prop_increment_by_payload:
            *bcm_type = bcmCpriRoEIncrTypePayloadSize;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}
#endif
int
bcm_esw_cpri_port_encap_roe_ordering_info_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_encap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_encap_ordering_info_entry_t_init(&entry);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_info_get(unit, port, index, &entry));

    _bcm_cprimod_to_bcm_ordering_info_type(entry.type, &ordering_info->info_type);
    _bcm_cprimod_to_bcm_ordering_info_incr_type(entry.pcnt_prop, &ordering_info->p_seq_incr);
    _bcm_cprimod_to_bcm_ordering_info_incr_type(entry.qcnt_prop, &ordering_info->q_seq_incr);
    ordering_info->pcnt_size        = entry.pcnt_size;
    ordering_info->pcnt_increment   = entry.pcnt_increment;
    ordering_info->pcnt_max         = entry.pcnt_max;
    ordering_info->qcnt_size        = entry.qcnt_size;
    ordering_info->qcnt_increment   = entry.qcnt_increment;
    ordering_info->qcnt_max         = entry.qcnt_max;
    ordering_info->seq_rsvd         = entry.seq_reserve_value;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_ordering_info_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_cpri_encap_roe_oi_t *ordering_info)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_ordering_info_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_encap_ordering_info_entry_t_init(&entry);

    _bcm_bcm_to_cprimod_ordering_info_type(ordering_info->info_type, &entry.type);
    _bcm_bcm_to_cprimod_ordering_info_incr_type(ordering_info->p_seq_incr, &entry.pcnt_prop);
    _bcm_bcm_to_cprimod_ordering_info_incr_type(ordering_info->q_seq_incr, &entry.qcnt_prop);
    entry.pcnt_size             = ordering_info->pcnt_size;
    entry.pcnt_increment        = ordering_info->pcnt_increment;
    entry.pcnt_max              = ordering_info->pcnt_max;
    entry.qcnt_size             = ordering_info->qcnt_size;
    entry.qcnt_increment        = ordering_info->qcnt_increment;
    entry.qcnt_max              = ordering_info->qcnt_max;
    entry.seq_reserve_value     = ordering_info->seq_rsvd;


    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_info_set(unit, port, index, &entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_encap_roe_ordering_sequence_offset_get(
    int unit,
    bcm_gport_t port,
    int queue,
    int control,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    /* user need to provide the sizes. */
    offset.p_size   = seq_offset->pcnt_size;
    offset.q_size   = seq_offset->qcnt_size;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_sequence_offset_get(unit, port, queue, control, &offset));

    seq_offset->pcnt_offset = offset.p_offset;
    seq_offset->qcnt_offset = offset.q_offset;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_roe_ordering_sequence_offset_set(
    int unit,
    bcm_gport_t port,
    int queue,
    int control,
    bcm_cpri_roe_oi_seq_offset_t *seq_offset)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_cpri_ordering_info_offset_t offset;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    offset.p_size   = seq_offset->pcnt_size;
    offset.p_offset = seq_offset->pcnt_offset;
    offset.q_size   = seq_offset->qcnt_size;
    offset.q_offset = seq_offset->qcnt_offset;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_roe_ordering_sequence_offset_set(unit, port, queue, control, &offset));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_to_cprimod_hdr_vlan_type(bcm_cpri_vlan_type_t bcm_type, cprimod_hdr_vlan_type_t *cprimod_type)
{
    switch (bcm_type) {
        case bcmCpriVlanTypeNone:
            *cprimod_type = cprimodHdrVlanTypeUntagged;
            break;

        case bcmCpriVlanTypeSingle:
            *cprimod_type = cprimodHdrVlanTypeTaggedVlan0;
            break;

        case bcmCpriVlanTypeQinQ:
            *cprimod_type = cprimodHdrVlanTypeQinQ;
            break;
        case bcmCpriVlanTypeSingleTable1:
            *cprimod_type = cprimodHdrVlanTypeTaggedVlan1;
            break;
        default:
            *cprimod_type = cprimodHdrVlanTypeUntagged;
        break;
    }
    return SOC_E_NONE;
}
#endif

int
bcm_esw_cpri_port_encap_set(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_cpri_encap_info_t *encap_config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_encap_header_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_encap_header_config_t_init(&config);

    _bcm_to_cprimod_hdr_vlan_type(encap_config->vlan_type, &config.vlan_type);

    config.header_type = encap_config->hdr_type ;

    config.roe_flow_id          = encap_config->roe_flow_id ;
    config.roe_subtype          = encap_config->subtype ;
    config.ordering_info_index  = encap_config->roe_ordering_info_index ;
    config.mac_da_index         = encap_config->macda_index ;
    config.mac_sa_index         = encap_config->macsa_index ;
    config.vlan_id_0_index      = encap_config->vlan0_idx ;
    config.vlan_id_1_index      = encap_config->vlan1_idx ;
    config.vlan_0_priority      = encap_config->vlan0_priority ;
    config.vlan_1_priority      = encap_config->vlan1_priority ;
    config.vlan_eth_type_index  = encap_config->vlan_ethertype_idx;
    config.roe_opcode           = encap_config->opcode;

    if(encap_config->flags & BCM_CPRI_ENCAP_USE_TAG_ID_FOR_VLAN){
        config.use_tagid_for_vlan = 1;
    } else {
        config.use_tagid_for_vlan = 0;
    }

    if(encap_config->flags & BCM_CPRI_ENCAP_USE_TAG_ID_FOR_FLOWID){
        config.use_tagid_for_flowid = 1;
    } else {
        config.use_tagid_for_flowid = 0;
    }

    if(encap_config->flags & BCM_CPRI_ENCAP_USE_OPCODE){
        config.use_opcode = 1;
    } else {
        config.use_opcode = 0;
    }
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_set(unit, port, queue_num, &config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_vlan_config_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_encap_vlan_config_t *vlan_config)
{
#ifdef CPRIMOD_SUPPORT
    uint16 ethertype = 0;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeRoe, &ethertype));
    vlan_config->roe_ethertype_0 = ethertype;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeRoe1, &ethertype));
    vlan_config->roe_ethertype_1= ethertype;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeVlan, &ethertype));
    vlan_config->ethertype_vlan_tagged= ethertype;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_get(unit, port, cprimodEthertypeQinQ, &ethertype));
    vlan_config->ethertype_vlan_qinq= ethertype;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_encap_vlan_config_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_encap_vlan_config_t *vlan_config)
{
#ifdef CPRIMOD_SUPPORT
    uint16 ethertype;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    ethertype = vlan_config->roe_ethertype_0;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeRoe, ethertype));

    ethertype = vlan_config->roe_ethertype_1;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeRoe1, ethertype));

    ethertype = vlan_config->ethertype_vlan_tagged;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeVlan, ethertype));

    ethertype = vlan_config->ethertype_vlan_qinq;
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_encap_ethertype_config_set(unit, port, cprimodEthertypeQinQ, ethertype));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_dbm_entry_clear(
    int unit,
    bcm_gport_t port,
    int index)
{
    return BCM_E_UNAVAIL;
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_to_cprimod_position_start(uint8 bcm_pos_index, cprimod_dbm_rule_pos_index_t *pos_grp_index)
{
    switch (bcm_pos_index) {
        case 0:
            *pos_grp_index = cprimodDbmRulePosIndexStart0;
            break;
        case 10:
            *pos_grp_index = cprimodDbmRulePosIndexStart10;
            break;
        case 20:
            *pos_grp_index = cprimodDbmRulePosIndexStart20;
            break;
        case 30:
            *pos_grp_index = cprimodDbmRulePosIndexStart30;
            break;
        case 40:
            *pos_grp_index = cprimodDbmRulePosIndexStart40;
            break;
        case 50:
            *pos_grp_index = cprimodDbmRulePosIndexStart50;
            break;
        case 60:
            *pos_grp_index = cprimodDbmRulePosIndexStart60;
            break;
        case 70:
            *pos_grp_index = cprimodDbmRulePosIndexStart70;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META("DBM Profile Position Table cannot start at %d\n"), bcm_pos_index));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

int _cprimod_to_bcm_position_start(cprimod_dbm_rule_pos_index_t pos_grp_index, uint8 *bcm_pos_index)
{
    switch (pos_grp_index) {
        case cprimodDbmRulePosIndexStart0:
            *bcm_pos_index = 0;
            break;
        case cprimodDbmRulePosIndexStart10:
            *bcm_pos_index = 10;
            break;
        case cprimodDbmRulePosIndexStart20:
            *bcm_pos_index = 20;
            break;
        case cprimodDbmRulePosIndexStart30:
            *bcm_pos_index = 30;
            break;
        case cprimodDbmRulePosIndexStart40:
            *bcm_pos_index = 40;
            break;
        case cprimodDbmRulePosIndexStart50:
            *bcm_pos_index = 50;
            break;
        case cprimodDbmRulePosIndexStart60:
            *bcm_pos_index = 60;
            break;
        case cprimodDbmRulePosIndexStart70:
            *bcm_pos_index = 70;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META( "DBM Profile Position Table cannot start at %d\n"), pos_grp_index));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

static
int _bcm_to_cprimod_position_table_size(uint8 pos_table_size, cprimod_dbm_rule_pos_grp_size_t *pos_grp_size)
{
    switch (pos_table_size) {
        case 10:
            *pos_grp_size = cprimodDbmRulePosGrpSize10;
            break;
        case 20:
            *pos_grp_size = cprimodDbmRulePosGrpSize20;
            break;
        case 40:
            *pos_grp_size = cprimodDbmRulePosGrpSize40;
            break;
        case 80:
            *pos_grp_size = cprimodDbmRulePosGrpSize80;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META( "DBM Profile Num Position Table cannot be  %d\n"), pos_table_size));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

static
int _cprimod_to_bcm_position_table_size(cprimod_dbm_rule_pos_grp_size_t pos_grp_size, uint8 *pos_table_size)
{
    switch (pos_grp_size) {
        case cprimodDbmRulePosGrpSize10:
            *pos_table_size = 10;
            break;
        case cprimodDbmRulePosGrpSize20:
            *pos_table_size = 20;
            break;
        case cprimodDbmRulePosGrpSize40:
            *pos_table_size = 40;
            break;
        case cprimodDbmRulePosGrpSize80:
            *pos_table_size = 80;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META( "DBM Profile Num Position Table cannot be  %d\n"), pos_grp_size));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}
#endif

int
bcm_esw_cpri_port_rsvd4_dbm_entry_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_rule_entry_t profile;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_rule_entry_t_init(&profile);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_entry_get(unit, port, index, &profile));
    entry->bm1_multiple = profile.bm1_mult;
    entry->bm1[0]       = profile.bm1[0];
    entry->bm1[1]       = profile.bm1[1];
    entry->bm1[2]       = profile.bm1[2];
    entry->bm1_size     = profile.bm1_size;
    entry->bm2[0]       = profile.bm2[0];
    entry->bm2[1]       = profile.bm2[1];
    entry->bm2_size     = profile.bm2_size;
    entry->num_slots    = profile.num_slots;

    SOC_IF_ERROR_RETURN
        (_cprimod_to_bcm_position_start(profile.pos_grp_index, &entry->pos_index));

    SOC_IF_ERROR_RETURN
        (_cprimod_to_bcm_position_table_size(profile.pos_grp_size,&entry->pos_entries));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_rsvd4_dbm_entry_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_rule_entry_t profile;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_rule_entry_t_init(&profile);

    profile.bm1_mult    = entry->bm1_multiple;
    profile.bm1[0]      = entry->bm1[0];
    profile.bm1[1]      = entry->bm1[1];
    profile.bm1[2]      = entry->bm1[2];
    profile.bm1_size    = entry->bm1_size;
    profile.bm2[0]      = entry->bm2[0];
    profile.bm2[1]      = entry->bm2[1];
    profile.bm2_size    = entry->bm2_size;
    profile.num_slots   = entry->num_slots;

    SOC_IF_ERROR_RETURN
        (_bcm_to_cprimod_position_start(entry->pos_index, &profile.pos_grp_index));

    SOC_IF_ERROR_RETURN
        (_bcm_to_cprimod_position_table_size(entry->pos_entries, &profile.pos_grp_size));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_entry_set(unit, port, index, &profile));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_bfn_sync_config_set(
    int unit,
    bcm_gport_t port,
    uint16 master_frame_count,
    uint64 master_frame_start)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_rx_master_frame_sync_config_set(unit, port, master_frame_count,master_frame_start));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef CPRIMOD_SUPPORT
static
int _bcm_to_cprimod_rsvd4_frame_sync_mode(bcm_rsvd4_sync_mode_t bcm_mode, cprimod_rsvd4_frame_sync_mode_t *cprimod_mode)
{
    switch (bcm_mode) {
        case bcmRsvd4SyncModeNoSync:
            *cprimod_mode = cprimodRsvd4FrameSyncModeNoSync;
            break;
        case bcmRsvd4SyncModeRadioFrame: /* use Msg Offset */
            *cprimod_mode = cprimodRsvd4FrameSyncModeMsgOffset;
            break;
        case bcmRsvd4SyncModeMasterFrame:
            *cprimod_mode = cprimodRsvd4FrameSyncModeMsgAndMasterOffset;
            break;
        case bcmRsvd4SyncModeTsNoSync:
            *cprimod_mode = cprimodRsvd4FrameSyncModeMsgTsNoSync;
            break;
        case bcmRsvd4SyncModeTsRadioFrame:
            *cprimod_mode = cprimodRsvd4FrameSyncModeMsgTsMsgOffset;
            break;
        case bcmRsvd4SyncModeTsMasterFrame:
            *cprimod_mode = cprimodRsvd4FrameSyncModeMsgTsMsgAndMasterOffset;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META( "mode out of range  %d\n"), bcm_mode));
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}
#endif

int
bcm_esw_cpri_port_rsvd4_rx_flow_add(
    int unit,
    bcm_gport_t port,
    int flow_id,
    bcm_rsvd4_rx_flow_info_t *flow_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_rx_flow_info_t int_flow_info;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    int_flow_info.mask                      = flow_info->mask ;
    int_flow_info.match_data                = flow_info->match_data ;

    int_flow_info.packet_size               = flow_info->payload_size ;
    int_flow_info.last_packet_size          = flow_info->last_payload_size ;
    int_flow_info.last_packet_num           = flow_info->last_payload_index ;

    int_flow_info.axc_id                 = flow_info->axc_id;
    int_flow_info.stuffing_cnt              = flow_info->stuffing_count ;
    int_flow_info.msg_ts_mode               = flow_info->msg_ts_mode; /*cprimod_rsvd4_msg_ts_mode_t and bcm_rsvd4_msg_ts_mode_t are identical */
    int_flow_info.msg_ts_count              = flow_info-> msg_ts_count;

    int_flow_info.master_frame_offset       = flow_info->master_frame_offset;
    int_flow_info.message_number_offset     = flow_info->message_number_offset ;
    int_flow_info.container_block_count     = flow_info->container_blk_count;
    int_flow_info.queue_size                = flow_info->queue_size ;
    int_flow_info.priority                  = flow_info->priority ;
    int_flow_info.pad_size                  = flow_info->gsm_pad_size ;
    int_flow_info.extra_pad_size            = flow_info->gsm_extra_pad_size ;
    int_flow_info.pad_enable                = flow_info->gsm_pad_en;
    int_flow_info.control_location          = flow_info->gsm_ctrl_loc ;
    int_flow_info.tsn_bitmap                = flow_info->gsm_tsn_bitmap ;

    SOC_IF_ERROR_RETURN
        (_bcm_to_cprimod_rsvd4_frame_sync_mode(flow_info->sync_mode, &int_flow_info.frame_sync_mode));

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_rx_flow_add(unit, port, flow_id, &int_flow_info));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_flow_delete(
    int unit,
    bcm_gport_t port,
    int flow_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_rsvd4_rx_frame_config_debug_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_config_t config_type,
    uint32 value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_rx_config_field_t field;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (config_type) {
    case bcmRsvd4RxConfigDescrambleEnable:
        field = cprimodRsvd4RxConfigDescrambleEnable;
        break;
    case bcmRsvd4RxConfigSyncThreshold:
        field = cprimodRsvd4RxConfigSyncThreshold;
        break;
    case bcmRsvd4RxConfigUnsyncThreshold:
        field = cprimodRsvd4RxConfigUnsyncThreshold;
        break;
    case bcmRsvd4RxConfigFrameSyncThreshold:
        field = cprimodRsvd4RxConfigFrameSyncThreshold;
        break;
    case bcmRsvd4RxConfigFrameUnsyncThreshold:
        field = cprimodRsvd4RxConfigFrameUnsyncThreshold;
        break;
    case bcmRsvd4RxConfigBlockSize:
        field = cprimodRsvd4RxConfigBlockSize;
        break;
    default:
        field = cprimodRsvd4RxConfigDescrambleEnable;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_rx_frame_optional_config_set(unit,
                                                                       port,
                                                                       field,
                                                                       value));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_frame_config_get(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_rx_speed_get(unit, port, &speed));
    if(speed == cprimodRsvd4SpdMult4X) {
        config->speed_multiple = 4;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        config->speed_multiple = 8;
    } else {
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_frame_config_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed = cprimodRsvd4SpdMultCount;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if(config->speed_multiple == 4) {
        speed = cprimodRsvd4SpdMult4X;
    } else if(config->speed_multiple == 8) {
        speed = cprimodRsvd4SpdMult8X;
    }
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_rx_speed_set(unit, port, speed));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_override_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_params_t parameter,
    int enable,
    int value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_rx_overide_t parameter_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (parameter) {
    case bcmRsvd4RxParams_Frame_Unsync_T_Invalid_MG_Rcvd:
        parameter_t = cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd;
        break;
    case bcmRsvd4RxParams_Frame_Sync_T_Valid_MG_Rcvd:
        parameter_t = cprimodRsvd4RxOverideFrameSyncTVldMgRecvd;
        break;
    case bcmRsvd4RxParams_K_MG_Idles_Rcvd:
        parameter_t = cprimodRsvd4RxOverideKMgIdlesRecvd;
        break;
    case bcmRsvd4RxParams_Idle_Req_Rcvd:
        parameter_t = cprimodRsvd4RxOverideIdleReqRecvd;
        break;
    case bcmRsvd4RxParams_Idle_Ack_Rcvd:
        parameter_t = cprimodRsvd4RxOverideIdleAckRecvd;
        break;
    case bcmRsvd4RxParams_SeedCapAndVerifyDone:
        parameter_t = cprimodRsvd4RxOverideSeedCapAndVerifyDone;
        break;
    default:
        parameter_t = cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_rx_override_set(unit, port,
                                                                parameter_t,
                                                                enable,
                                                                value));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_rx_fsm_state_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_rx_fsm_state_t state)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_rx_fsm_state_t state_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (state) {
    case bcmRsvd4RxFsmStateUnsync:
        state_t = cprimodRsvd4RxFsmStateUnsync;
        break;
    case bcmRsvd4RxFsmStateWaitForSeed:
        state_t = cprimodRsvd4RxFsmStateWaitForSeed;
        break;
    case bcmRsvd4RxFsmStateWaitForK28p7Idles:
        state_t = cprimodRsvd4RxFsmStateWaitForK28p7Idles;
        break;
    case bcmRsvd4RxFsmStateWaitForFrameSync:
        state_t = cprimodRsvd4RxFsmStateWaitForFrameSync;
        break;
    case bcmRsvd4RxFsmStateFrameSync:
        state_t = cprimodRsvd4RxFsmStateFrameSync;
        break;
    default:
        state_t = cprimodRsvd4RxFsmStateUnsync;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_rx_fsm_state_set(unit, port,
                                                                 state_t));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_secondary_dbm_entry_clear(
    int unit,
    bcm_gport_t port,
    int index)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_rsvd4_secondary_dbm_entry_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_secondary_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_secondary_dbm_rule_entry_t profile;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_secondary_dbm_rule_entry_t_init(&profile);

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_secondary_dbm_entry_set(unit, port, index, &profile));

    entry->bm1_multiple     = profile.bm1_mult;
    entry->bm1              = profile.bm1;
    entry->bm1_size         = profile.bm1_size;
    entry->bm2              = profile.bm2;
    entry->bm2_size         = profile.bm2_size;
    entry->num_slots        = profile.num_slots;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif

}

int
bcm_esw_cpri_port_rsvd4_secondary_dbm_entry_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_rsvd4_secondary_dbm_entry_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_secondary_dbm_rule_entry_t profile;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_secondary_dbm_rule_entry_t_init(&profile);

    profile.bm1_mult    = entry->bm1_multiple;
    profile.bm1         = entry->bm1;
    profile.bm1_size    = entry->bm1_size;
    profile.bm2         = entry->bm2;
    profile.bm2_size    = entry->bm2_size;
    profile.num_slots   = entry->num_slots;

    SOC_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_secondary_dbm_entry_set(unit, port, index, &profile));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_add(
    int unit,
    bcm_gport_t port,
    int flow_id,
    bcm_rsvd4_tx_flow_info_t *flow_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_rsvd4_tx_flow_info_t int_flow_info;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    portmod_rsvd4_tx_flow_info_t_init(unit, &int_flow_info);

    int_flow_info.packet_size               = flow_info->payload_size;
    int_flow_info.last_packet_size          = flow_info->last_payload_size;
    int_flow_info.last_packet_num           = flow_info->last_index ;

    int_flow_info.axc_id                    = flow_info->axc_id;
    int_flow_info.stuffing_cnt              = flow_info->stuffing_count ;
    int_flow_info.msg_ts_mode               = flow_info->msg_ts_mode; /*cprimod_rsvd4_msg_ts_mode_t and bcm_rsvd4_msg_ts_mode_t are identical */
    int_flow_info.msg_ts_count              = flow_info-> msg_ts_count;
    int_flow_info.master_frame_offset       = flow_info->master_frame_offset;
    int_flow_info.message_number_offset     = flow_info->message_number_offset;
    int_flow_info.container_block_count     = flow_info->container_blk_count;
    int_flow_info.buffer_size                = flow_info->buffer_size ;
    int_flow_info.cycle_size                = flow_info->cycle_size ;
    int_flow_info.use_ts_dbm                = flow_info->secondary_dbm_enable;
    int_flow_info.ts_dbm_profile_num        = flow_info->index_to_secondary_dbm; /* secondary dbm profile num. */
    int_flow_info.num_active_slot           = flow_info->num_active_slot;
    int_flow_info.msg_addr                  = flow_info->message_addr;
    int_flow_info.msg_type                  = flow_info->message_type;
    int_flow_info.msg_ts_offset             = flow_info->msg_ts_offset;
    int_flow_info.pad_size                  = flow_info->pad_size ;
    int_flow_info.extra_pad_size            = flow_info->extra_pad_size ;
    int_flow_info.pad_enable                = flow_info->pad_enable;
    int_flow_info.control_location          = flow_info->gsm_ctrl_loc ;

    SOC_IF_ERROR_RETURN
        (_bcm_to_cprimod_rsvd4_frame_sync_mode(flow_info->sync_mode, &int_flow_info.frame_sync_mode));

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_tx_flow_add(unit, port, flow_id, &int_flow_info));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_delete(
    int unit,
    bcm_gport_t port,
    int flow_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_rule_clear(
    int unit,
    bcm_gport_t port,
    int flow_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_rule_get(
    int unit,
    bcm_gport_t port,
    int flow_id,
    bcm_rsvd4_tx_flow_rule_t *flow_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_rsvd4_tx_flow_rule_set(
    int unit,
    bcm_gport_t port,
    int flow_id,
    bcm_rsvd4_tx_flow_rule_t *config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cpri_port_rsvd4_tx_modulo_rule_entry_set(
    int unit,
    bcm_gport_t port,
    uint32 modulo_rule_num,
    bcm_cpri_tx_rule_type_t modulo_rule_type,
    bcm_cpri_modulo_rule_entry_t* modulo_rule)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_modulo_rule_entry_t mod_rule;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_modulo_rule_entry_t_init(&mod_rule);

    mod_rule.active         = modulo_rule->active;
    mod_rule.modulo_value   = modulo_rule->modulo_value;
    mod_rule.modulo_index   = modulo_rule->modulo_index;
    mod_rule.dbm_enable     = modulo_rule->dbm_enable;
    mod_rule.flow_dbm_id    = modulo_rule->dbm_dbm_id;
    mod_rule.flow_type      = modulo_rule->flow_type;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_modulo_rule_entry_set(unit, port, modulo_rule_num, modulo_rule_type, &mod_rule));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_modulo_rule_entry_get(
    int unit,
    bcm_gport_t port,
    uint32 modulo_rule_num,
    bcm_cpri_tx_rule_type_t modulo_rule_type,
    bcm_cpri_modulo_rule_entry_t* modulo_rule)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_modulo_rule_entry_t mod_rule;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_modulo_rule_entry_t_init(&mod_rule);
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_modulo_rule_entry_get(unit, port, modulo_rule_num, modulo_rule_type, &mod_rule));
    modulo_rule->active         = mod_rule.active;
    modulo_rule->modulo_value   = mod_rule.modulo_value;
    modulo_rule->modulo_index   = mod_rule.modulo_index;
    modulo_rule->dbm_enable     = mod_rule.dbm_enable;
    modulo_rule->dbm_dbm_id     = mod_rule.flow_dbm_id ;
    modulo_rule->flow_type      = mod_rule.flow_type;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_position_entry_set(
    int unit,
    bcm_gport_t port,
    uint32 pos_index,
    bcm_cpri_dbm_position_entry_t* pos_entry)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_pos_table_entry_t position_entry;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_pos_table_entry_t_init(&position_entry);

    position_entry.valid        = pos_entry->valid;
    position_entry.flow_id      = pos_entry->flow_id;
    position_entry.flow_type    = pos_entry->flow_type;
    position_entry.index        = pos_entry->index;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_position_entry_set(unit, port, pos_index, &position_entry));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_position_entry_get(
    int unit,
    bcm_gport_t port,
    uint32 pos_index,
    bcm_cpri_dbm_position_entry_t* pos_entry)

{
#ifdef CPRIMOD_SUPPORT
    cprimod_dbm_pos_table_entry_t position_entry;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    cprimod_dbm_pos_table_entry_t_init(&position_entry);

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rsvd4_dbm_position_entry_set(unit, port, pos_index, &position_entry));

    pos_entry->valid        = position_entry.valid;
    pos_entry->flow_id      = position_entry.flow_id;
    pos_entry->flow_type    = position_entry.flow_type;
    pos_entry->index        = position_entry.index;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_frame_config_debug_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_config_t config_type,
    uint32 value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_tx_config_field_t field;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (config_type) {
    case bcmRsvd4TxConfigForceOffState:
        field = cprimodRsvd4TxConfigForceOffState;
        break;
    case bcmRsvd4TxConfigAckT:
        field = cprimodRsvd4TxConfigAckT;
        break;
    case bcmRsvd4TxConfigForceIdleAck:
        field = cprimodRsvd4TxConfigForceIdleAck;
        break;
    case bcmRsvd4TxConfigLosEnable:
        field = cprimodRsvd4TxConfigLosEnable;
        break;
    case bcmRsvd4TxConfigScrambleEnable:
        field = cprimodRsvd4TxConfigScrambleEnable;
        break;
    case bcmRsvd4TxConfigTransmitterEnable:
        field = cprimodRsvd4TxConfigTransmitterEnable;
        break;
    default:
        field = cprimodRsvd4TxConfigForceOffState;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                                       port,
                                                                       field,
                                                                       value));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_frame_config_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_frame_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_port_rsvd4_speed_mult_t speed = cprimodRsvd4SpdMultCount;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    if(config->speed_multiple == 4) {
        speed = cprimodRsvd4SpdMult4X;
    } else if(config->speed_multiple == 8) {
        speed = cprimodRsvd4SpdMult8X;
    }

    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_speed_set(unit, port, speed));
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                                       port,
                                                                       cprimodRsvd4TxConfigScramblerSeed,
                                                                       config->rsvd4_scrambler_seed));
    BCM_IF_ERROR_RETURN(portmod_port_rsvd4_tx_frame_optional_config_set(unit,
                                                                       port,
                                                                       cprimodRsvd4TxConfigLosEnable,
                                                                       config->los_enable));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_override_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_params_t parameter,
    int enable,
    int value)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_tx_overide_t parameter_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (parameter) {
    case bcmRsvd4TxParams_StartTx:
        parameter_t = cprimodRsvd4TxOverideStartTx;
        break;
    case bcmRsvd4TxParams_PCS_Ack_CAP:
        parameter_t = cprimodRsvd4TxOverideRxPcsAckCap;
        break;
    case bcmRsvd4TxParams_PCS_Idle_REQ:
        parameter_t = cprimodRsvd4TxOverideRxPcsIdleReq;
        break;
    case bcmRsvd4TxParams_PCS_SCR_Lock:
        parameter_t = cprimodRsvd4TxOverideRxPcsScrLock;
        break;
    case bcmRsvd4TxParams_LOS_Status:
        parameter_t = cprimodRsvd4TxOverideLosStauts;
        break;
    default:
        parameter_t = cprimodRsvd4TxOverideStartTx;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_tx_override_set(unit, port,
                                                                parameter_t,
                                                                enable,
                                                                value));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rsvd4_tx_fsm_state_set(
    int unit,
    bcm_gport_t port,
    bcm_rsvd4_tx_fsm_state_t state)
{
#ifdef CPRIMOD_SUPPORT
    cprimod_rsvd4_tx_fsm_state_t state_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (state) {
    case bcmRsvd4TxFsmStateOff:
        state_t = cprimodRsvd4TxFsmStateOff;
        break;
    case bcmRsvd4TxFsmStateIdle:
        state_t = cprimodRsvd4TxFsmStateIdle;
        break;
    case bcmRsvd4TxFsmStateIdleReq:
        state_t = cprimodRsvd4TxFsmStateIdleReq;
        break;
    case bcmRsvd4TxFsmStateIdleAck:
        state_t = cprimodRsvd4TxFsmStateIdleAck;
        break;
    case bcmRsvd4TxFsmStateFrameTx:
        state_t = cprimodRsvd4TxFsmStateFrameTx;
        break;
    default:
        state_t = cprimodRsvd4TxFsmStateOff;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rsvd4_tx_fsm_state_set(unit, port,
                                                                 state_t));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_add(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    _bcm_cpri_axc_frame_info_t_to_portmod_cpri_axc_frame_info_t(axc_id,
                                                                axc_info,
                                                                &axc_info_t);
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_add(unit, port, axc_id,
                                                           &axc_info_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_delete(
    int unit,
    bcm_gport_t port,
    int axc_id)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_delete(unit, port,
                                                              axc_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_get(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_get(unit, port, axc_id,
                                                           &axc_info_t));
    _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(&axc_info_t, axc_info);

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_axc_frame_get_all(
    int unit,
    bcm_gport_t port,
    int axc_id,
    int max_count,
    bcm_cpri_axc_frame_info_t *axc_info_list,
    int *axc_count)
{
#ifdef CPRIMOD_SUPPORT
    int i;
    portmod_cpri_axc_frame_info_t axc_info_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    for (i=0; i<max_count; i++) {
        BCM_IF_ERROR_RETURN(portmod_cpri_port_rx_axc_frame_get(unit, port,
                                                               i,
                                                               &axc_info_t));
        _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(&axc_info_t,
                                                                    axc_info_list);
        axc_info_list++;
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t *port_type,
    bcm_cpri_port_speed_t *speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_get(unit, port, &config,
                                                          PORTMOD_INIT_F_RX_ONLY));
    *speed = _int_to_bcm_cpri_port_speed_t(config.speed);
    *port_type = bcmCpriPortTypeCpri;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_rx_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t port_type,
    bcm_cpri_port_speed_t speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    config.speed = _bcm_cpri_port_speed_t_to_int(speed);
    config.encap_mode = _SHR_PORT_ENCAP_CPRI;
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_set(unit, port, &config,
                                                          PORTMOD_INIT_F_RX_ONLY));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_stage_activate(
    int unit,
    bcm_gport_t port,
    bcm_cpri_stage_t stage)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_stage_t stage_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    switch (stage) {
    case bcmCpriStageRxParser:
        stage_t = portmodCpriStageRx;
        break;
    case bcmCpriStageTxAssembler:
        stage_t = portmodCpriStageTx;
        break;
    default:
        stage_t = portmodCpriStageRx;
        break;
    }
    BCM_IF_ERROR_RETURN(portmod_cpri_port_stage_activate(unit, port, stage_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_add(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    _bcm_cpri_axc_frame_info_t_to_portmod_cpri_axc_frame_info_t(axc_id,
                                                                axc_info,
                                                                &axc_info_t);
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_add(unit, port, axc_id,
                                                           &axc_info_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_delete(
    int unit,
    bcm_gport_t port,
    int axc_id)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_delete(unit, port,
                                                              axc_id));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_get(
    int unit,
    bcm_gport_t port,
    int axc_id,
    bcm_cpri_axc_frame_info_t *axc_info)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_axc_frame_info_t axc_info_t;
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_get(unit, port, axc_id,
                                                           &axc_info_t));
    _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(&axc_info_t, axc_info);

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_axc_frame_get_all(
    int unit,
    bcm_gport_t port,
    int axc_id,
    /* axc_id should be an array Add in PAPI*/
    int max_count,
    bcm_cpri_axc_frame_info_t *axc_info_list,
    int *axc_count)
{
#ifdef CPRIMOD_SUPPORT
    int i;
    portmod_cpri_axc_frame_info_t axc_info_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    for (i=0; i<max_count; i++) {
        BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_axc_frame_get(unit, port,
                                                               i,
                                                               &axc_info_t));
        _portmod_cpri_axc_frame_info_t_to_bcm_cpri_axc_frame_info_t(&axc_info_t,
                                                                    axc_info_list);
        axc_info_list++;
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_frame_tgen_config_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_tx_frame_tgen_config_t *tgen_config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_tx_frame_tgen_config_t tgen_cfg_t;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    tgen_cfg_t.tx_tgen_hfn = tgen_config->tx_tgen_hfn ;
    tgen_cfg_t.tx_tgen_bfn = tgen_config->tx_tgen_bfn;
    tgen_cfg_t.tx_gen_bfn_hfn_sel = tgen_config->tx_tgen_bfn_sel;
    COMPILER_64_COPY(tgen_cfg_t.tx_tgen_ts_offset, tgen_config->tx_tgen_ts_offset);
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_frame_tgen_config_set(unit, port,
                                                                   &tgen_cfg_t));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_frame_tgen_enable(
    int unit,
    bcm_gport_t port,
    int enable)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_cpri_port_tx_frame_tgen_enable(unit, port,
                                                               enable));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t *port_type,
    bcm_cpri_port_speed_t *speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_get(unit, port, &config,
                                                          PORTMOD_INIT_F_TX_ONLY));
    *speed = _int_to_bcm_cpri_port_speed_t(config.speed);
    *port_type = bcmCpriPortTypeCpri;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_esw_cpri_port_tx_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_cpri_port_type_t port_type,
    bcm_cpri_port_speed_t speed)
{
#ifdef CPRIMOD_SUPPORT
    portmod_port_interface_config_t config;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    config.speed = _bcm_cpri_port_speed_t_to_int(speed);
    config.encap_mode = _SHR_PORT_ENCAP_CPRI;
    BCM_IF_ERROR_RETURN(portmod_port_interface_config_set(unit, port, &config,
                                                          PORTMOD_INIT_F_TX_ONLY));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Configure control message handling. */
extern int bcm_esw_cpri_port_rsvd4_rx_control_config_set(
    int unit,
    int port,
    bcm_rsvd4_control_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    /* will not implement this. */
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Add control message flow. */
int bcm_esw_cpri_port_rsvd4_rx_control_flow_add(
        int unit,
        int port,
        int control_flow_id,
        bcm_rsvd4_control_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_control_flow_config_t portmod_config;

    portmod_control_flow_config_t_init(unit, &portmod_config);

    portmod_config.match_mask   = config->match_mask ;
    portmod_config.match_data   = config->match_data ;
    portmod_config.proc_type    = config->type ;
    portmod_config.queue_num    = config->queue_num ;
    portmod_config.sync_profile = config->sync_profile ;
    portmod_config.sync_enable  = config->sync_en ;
    portmod_config.priority     = config->priority ;
    portmod_config.queue_size   = config->queue_size ;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_control_flow_add(unit, port, control_flow_id, &portmod_config));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Delete control message flow. */
int bcm_esw_cpri_port_rsvd4_rx_control_flow_delete(
    int unit,
    int port,
    int control_flow_id,
    bcm_rsvd4_control_flow_config_t *control_flow_cfg)
{

#ifdef CPRIMOD_SUPPORT
    portmod_control_flow_config_t portmod_config;

    portmod_control_flow_config_t_init(unit, &portmod_config);

    portmod_config.match_mask   = control_flow_cfg->match_mask ;
    portmod_config.match_data   = control_flow_cfg->match_data ;
    portmod_config.proc_type    = control_flow_cfg->type ;
    portmod_config.queue_num    = control_flow_cfg->queue_num ;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_control_flow_delete(unit, port, control_flow_id, &portmod_config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Setup the fast ethernet control flow. */
int bcm_esw_cpri_port_rsvd4_cm_fast_eth_config_set(
    int unit,
    int port,
    int control_flow_id,
    bcm_rsvd4_fast_eth_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_fast_eth_config_set(unit, port, config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Configure sync profile entry. */
int bcm_esw_cpri_port_rsvd4_sync_profile_entry_set(
    int unit,
    int port,
    int index,
    bcm_rsvd4_sync_profile_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_sync_profile_entry_t entry;

    portmod_sync_profile_entry_t_init(unit, &entry);
    entry.count_cycle           = config->count_cycle;
    entry.message_offset        = config->message_offset;
    entry.master_frame_offset   = config->master_frame_offset;

    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_sync_profile_entry_set(unit, port, index, &entry));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/* Retrieve sync profile entry. */
int bcm_esw_cpri_port_rsvd4_sync_profile_entry_get(
    int unit,
    int port,
    int index,
    bcm_rsvd4_sync_profile_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_sync_profile_entry_t entry;

    portmod_sync_profile_entry_t_init(unit, &entry);


    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_rx_sync_profile_entry_get(unit, port, index, &entry));

    config->count_cycle         = entry.count_cycle;
    config->message_offset      = entry.message_offset;
    config->master_frame_offset = entry.master_frame_offset;

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_rx_tag_gen_entry_add( /* PAPI need to remove */
    int unit,
    int port,
    int index,
    bcm_cpri_tag_gen_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_gen_entry_add(unit, port, config));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_control_flow_group_member_add(
    int unit,
    int port,
    bcm_rsvd4_control_group_id_t group_num,
    bcm_rsvd4_tx_control_flow_group_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_group_member_add(unit, port, group_num, config->priority, config->queue_num, config->type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_control_flow_group_member_delete ( 
    int unit,
    int port,
    bcm_rsvd4_control_group_id_t group_num, 
    bcm_rsvd4_priority_t priority) 
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_group_member_delete(unit, port, group_num, priority));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_eth_config_set(
    int unit,
    int port,
    bcm_rsvd4_tx_cm_eth_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_eth_message_config_set(unit, port, config->msg_node, config->msg_subnode, config->msg_type, config->msg_padding));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_raw_config_set(
    int unit,
    int port,
    bcm_rsvd4_tx_cm_raw_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_single_raw_message_config_set(unit, port,  config->msg_id, config->msg_type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_control_flow_add(
    int unit,
    int port,
    int control_flow_id,
    bcm_rsvd4_tx_control_flow_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_config_set(unit, port, control_flow_id, config->queue_num, config->type));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*  */
int bcm_esw_cpri_port_rsvd4_tx_cm_tunnel_crc_config_set(
    int unit,
    int port,
    bcm_rsvd4_cm_tunnel_crc_option_t crc_option)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_single_tunnel_message_config_set(unit, port, crc_option)); 
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_hdr_config_entry_set(
    int unit,
    int port,
    int index,
    bcm_rsvd4_tx_cm_hdr_entry_t* entry) 
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_header_entry_set(unit, port, index, entry->rsvd4_header_node,
                                                   entry->rsvd4_header_subnode, entry->rsvd4_control_payload_node));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rsvd4_tx_cm_hdr_lkup_entry_set(
    int unit,
    int port,
    int flow_id,
    int header_index)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rsvd4_tx_control_flow_header_index_set(unit, port, flow_id, header_index));
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

int bcm_esw_cpri_port_rx_cw_sync_info_get(
    int unit,
    int port,
    uint32* hyper_frame_num,
    uint32* radio_frame_num)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_sync_info_get(unit, port, hyper_frame_num, radio_frame_num));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_l1_signal_info_get(
    int unit,
    int port,
    bcm_cpri_cw_l1_protocol_info_t *l1_info)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_l1_signal_info_get(unit, port, l1_info));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_l1_signal_protection_set(
    int unit,
    int port,
    uint32 signal_map,
    uint32 enable)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_l1_signal_signal_protection_set(unit, port, signal_map, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_l1_signal_protection_get(
    int unit,
    int port,
    uint32 signal_map,
    uint32 *enable)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_cw_l1_signal_signal_protection_get(unit, port, signal_map, enable));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_slow_hdlc_config_set(
    int unit,
    int port,
    bcm_cpri_slow_hdlc_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_slow_hdlc_config_info_t portmod_config;

    portmod_cpri_slow_hdlc_config_info_t_init(unit, &portmod_config);

    portmod_config.tx_crc_mode      = config->hdlc_crc_mode;
    portmod_config.tx_flag_size     = config->tx_flag_size;
    portmod_config.crc_init_val     = config->crc_init_val;
    portmod_config.use_fe_mac       = config->use_fe_mac;
    portmod_config.crc_byte_swap    = config->crc_byte_swap;
    portmod_config.no_fcs_err_check = config->no_fcs_err_check;
    portmod_config.cw_sel           = config->hdlc_cw_sel;
    portmod_config.cw_size          = config->hdlc_cw_size;
    portmod_config.fcs_size         = config->hdlc_fcs_size;
    portmod_config.runt_frame_drop  = config->hdlc_run_drop;
    portmod_config.long_frame_drop  = config->hdlc_max_drop;
    portmod_config.min_frame_size   = config->hdlc_min_size;
    portmod_config.max_frame_size   = config->hdlc_max_size;
    portmod_config.queue_num        = config->hdlc_queue_num;
    portmod_config.priority         = config->priority;
    portmod_config.queue_size       = config->queue_size;
    portmod_config.cycle_size       = config->cycle_size;
    portmod_config.buffer_size      = config->buffer_size;

    portmod_config.tx_filling_flag_pattern  = config->tx_filling_pattern;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_cw_slow_hdlc_config_set(unit, port, &portmod_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_slow_hdlc_config_get(
    int unit,
    int port,
    bcm_cpri_slow_hdlc_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_fast_eth_config_set(
    int unit,
    int port,
    bcm_cpri_fast_eth_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_fast_eth_config_info_t portmod_config;

    portmod_cpri_fast_eth_config_info_t_init(unit, &portmod_config);

    portmod_config.tx_crc_mode      = config->crc_mode;
    portmod_config.schan_start      = config->sub_channel_start;
    portmod_config.schan_size       = config->sub_channel_size;
    portmod_config.cw_sel           = config->cw_sel;
    portmod_config.cw_size          = config->cw_size;
    portmod_config.min_packet_size  = config->min_pkt_size;
    portmod_config.max_packet_size  = config->max_pkt_size;
    portmod_config.min_packet_drop  = config->drop_min_pkt_size;
    portmod_config.max_packet_drop  = config->drop_max_pkt_size;
    portmod_config.strip_crc        = config->strip_crc;
    portmod_config.min_ipg          = config->min_ipg;
    portmod_config.queue_num        = config->queue_num;
    portmod_config.priority         = config->priority;
    portmod_config.queue_size       = config->queue_size;
    portmod_config.cycle_size       = config->cycle_size;
    portmod_config.buffer_size      = config->buffer_size;
    portmod_config.no_fcs_err_check = config->ignore_fcs_err;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_cw_fast_eth_config_set(unit, port, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_cw_fast_eth_config_get(
    int unit,
    int port,
    bcm_cpri_fast_eth_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_config_set(
    int unit,
    int port,
    bcm_cpri_rx_vsd_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_config_info_t portmod_config;

    portmod_cpri_rx_vsd_config_info_t_init(unit,&portmod_config);
    portmod_config.schan_start          = config->schan_start;
    portmod_config.schan_size           = config->schan_size;
    portmod_config.schan_bytes          = config->schan_bytes;
    portmod_config.schan_steps          = config->schan_step;
    portmod_config.flow_bytes           = config->flow_bytes;
    portmod_config.queue_num            = config->queue_num;
    portmod_config.rsvd_sector_mask[0]  = config->vsd_valid_sector_mask[0];
    portmod_config.rsvd_sector_mask[1]  = config->vsd_valid_sector_mask[1];
    portmod_config.rsvd_sector_mask[2]  = config->vsd_valid_sector_mask[2];
    portmod_config.rsvd_sector_mask[3]  = config->vsd_valid_sector_mask[3];
    portmod_config.queue_size           = config->queue_size;
    portmod_config.priority             = config->priority;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_config_set(unit, port, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_config_get(
    int unit,
    int port,
    bcm_cpri_rx_vsd_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_ctrl_flow_config_set( 
    int unit,
    int port,
    uint16 group_id,
    bcm_cpri_rx_vsd_flow_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_flow_add(unit, port, group_id, config)); 

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_ctrl_flow_config_get(
    int unit,
    int port,
    uint16 group_id,
    bcm_cpri_rx_vsd_flow_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_encap_control_queue_tag_to_flow_id_map_set(
    int unit,
    int port,
    uint32 tag_id,
    uint32 flow_id)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_encap_queue_control_tag_to_flow_map_set(unit, port, tag_id, flow_id));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_raw_config_set(
    int unit,
    int port,
    uint8 vsd_raw_id,
    bcm_cpri_rx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_vsd_raw_config_t portmod_config;

    portmod_cpri_rx_vsd_raw_config_t_init(unit,&portmod_config);

    portmod_config.schan_start          = config->schan_start;
    portmod_config.schan_size           = config->schan_size;
    portmod_config.cw_sel               = config->cw_sel;
    portmod_config.cw_size              = config->cw_size;
    portmod_config.filter_mode          = config->filter_mode;
    portmod_config.hyper_frame_index    = config->hfn_offset;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo;
    portmod_config.match_offset         = config->match_offset;
    portmod_config.match_value          = config->match_value;
    portmod_config.match_mask           = config->match_mask;
    portmod_config.queue_num            = config->queue_num;
    portmod_config.priority             = config->priority;
    portmod_config.queue_size           = config->queue_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_vsd_raw_config_set(unit, port, vsd_raw_id, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_vsd_raw_config_get(
    int unit,
    int port,
    uint8 vsd_raw_id,
    bcm_cpri_rx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_rsvd5_config_set(
    int unit,
    int port,
    bcm_cpri_rx_rsvd5_config_info_t* config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_brcm_rsvd5_config_t portmod_config;

    portmod_cpri_rx_brcm_rsvd5_config_t_init(unit,&portmod_config);
    portmod_config.schan_start      = config->schan_start ;
    portmod_config.schan_size       = config->schan_size ;
    portmod_config.parity_disable   = config->disable_parity_check ;
    portmod_config.queue_num        = config->queue_num;
    portmod_config.priority         = config->priority;
    portmod_config.queue_size       = config->queue_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_brcm_rsvd5_config_set(unit, port, &portmod_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_rsvd5_config_get(
    int unit,
    int port,
    bcm_cpri_rx_rsvd5_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_tx_tag_config_set( /* PAPI remove */
    int unit,
    int port,
    uint32 default_tag,
    uint32 no_match_tag)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}
/*  */
int bcm_esw_cpri_port_rx_tag_entry_add(
    int unit,
    int port,
    bcm_cpri_tag_gen_entry_t *config)

{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_gen_entry_add(unit, port, config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_tag_entry_delete(
    int unit,
    int port,
    bcm_cpri_tag_gen_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT

    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_gen_entry_delete(unit, port, config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_fec_enable_set(
    int unit,
    int port,
    uint8 enable)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_fec_tick_set(
    int unit,
    int port,
    bcm_cpri_fec_tick_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_fec_timestamp_config_set(
    int unit,
    int port,
    bcm_cpri_fec_timestamp_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_fec_soft_reset_set(
    int unit,
    int port,
    uint8 enable)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_fec_descramble_bypass_set(
    int unit,
    int port,
    uint8 enable)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_gcw_config_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_rx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_rx_gcw_config_t portmod_config;

    portmod_cpri_rx_gcw_config_t_init(unit,&portmod_config);
    portmod_config.Ns       = config->Ns ;
    portmod_config.Xs       = config->Xs ;
    portmod_config.Y        = config->Y ;
    portmod_config.mask     = config->mask ;
    portmod_config.filter_mode          = config->filter_mode ;
    portmod_config.hyper_frame_index    = config->hfn_index ;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo ;
    portmod_config.match_mask           = config->match_mask ;
    portmod_config.match_value          = config->match_value;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_rx_gcw_config_set(unit, port, index, &portmod_config));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_cw_gcw_config_get(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_rx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_sync_info_set(
    int unit,
    int port,
    bcm_cpri_cw_sync_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_cw_sync_info_set(unit, port, entry->hfn, entry->bfn));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_sync_info_get(
    int unit,
    int port,
    bcm_cpri_cw_sync_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_l1_config_set(
    int unit,
    int port,
    bcm_cpri_cw_l1_config_info_t *entry) 
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_cw_l1_config_set(unit, port, entry));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_config_set(
    int unit,
    int port,
    bcm_cpri_tx_vsd_config_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_vsd_config_info_t portmod_config;

    portmod_cpri_tx_vsd_config_info_t_init(unit,&portmod_config);
    portmod_config.schan_start          = entry->schan_start;
    portmod_config.schan_size           = entry->schan_size;
    portmod_config.schan_bytes          = entry->schan_bytes;
    portmod_config.schan_steps          = entry->schan_step;
    portmod_config.flow_bytes           = entry->flow_bytes;
    portmod_config.queue_num            = entry->queue;
    portmod_config.rsvd_sector_mask[0]  = entry->vsd_valid_sector_mask[0];
    portmod_config.rsvd_sector_mask[1]  = entry->vsd_valid_sector_mask[1];
    portmod_config.rsvd_sector_mask[2]  = entry->vsd_valid_sector_mask[2];
    portmod_config.rsvd_sector_mask[3]  = entry->vsd_valid_sector_mask[3];
    portmod_config.buffer_size          = entry->buffer_size;
    portmod_config.cycle_size           = entry->cycle_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_config_set(unit, port, &portmod_config));


    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_config_get(
    int unit,
    int port,
    bcm_cpri_tx_vsd_config_info_t *entry)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}
    
int bcm_esw_cpri_port_tx_cw_vsd_raw_flow_config_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_cpri_tx_vsd_raw_config_t portmod_config;

    portmod_cpri_tx_vsd_raw_config_t_init(unit,&portmod_config);

    portmod_config.schan_start          = config->schan_start;
    portmod_config.schan_size           = config->schan_size;
    portmod_config.cw_sel               = config->cw_sel;
    portmod_config.cw_size              = config->cw_size;
    portmod_config.map_mode             = config->map_mode;
    portmod_config.repeat_enable        = config->repeat_mode;
    portmod_config.hyper_frame_index    = config->hfn_index;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo;
    portmod_config.bfn0_filter_enable   = config->bfn0_filter_enable;
    portmod_config.bfn1_filter_enable   = config->bfn1_filter_enable;
    portmod_config.idle_value           = config->idle_value;
    portmod_config.queue_num            = config->queue_num;
    portmod_config.cycle_size           = config->cycle_size;
    portmod_config.buffer_size          = config->buffer_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_raw_config_set(unit, port, index, &portmod_config)) ;


    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_flow_config_get(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_vsd_raw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_filter_set(
    int unit,
    int port,
    bcm_cpri_tx_cw_vsd_raw_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_raw_filter_set(unit, port, config->bfn0_value,config->bfn0_mask, config->bfn1_value, config->bfn1_mask));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_raw_filter_get(
    int unit,
    int port,
    bcm_cpri_tx_cw_vsd_raw_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_rsvd5_config_set(
    int unit,
    int port,
    bcm_cpri_tx_cw_rsvd5_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT

    portmod_cpri_tx_brcm_rsvd5_config_t portmod_config;

    portmod_cpri_tx_brcm_rsvd5_config_t_init(unit,&portmod_config);

    portmod_config.schan_start  = config->schan_start ;
    portmod_config.schan_size   = config->schan_size ;
    portmod_config.crc_enable   = config->crc_check_enable;
    portmod_config.queue_num    = config->queue_num;
    portmod_config.buffer_size  = config->buffer_size;
    portmod_config.cycle_size   = config->cycle_size;

    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_brcm_rsvd5_config_set(unit, port, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_rsvd5_config_get(
    int unit,
    int port,
    bcm_cpri_tx_cw_rsvd5_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_gcw_config_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_gcw_config_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    portmod_cpri_tx_gcw_config_t portmod_config;

    portmod_cpri_tx_gcw_config_t_init(unit,&portmod_config);
    portmod_config.Ns       = config->Ns ;
    portmod_config.Xs       = config->Xs ;
    portmod_config.Y        = config->Y ;
    portmod_config.mask     = config->mask ;
    portmod_config.repeat_enable        = config->repeat_mode ;
    portmod_config.hyper_frame_index    = config->hfn_index ;
    portmod_config.hyper_frame_modulo   = config->hfn_modulo ;
    portmod_config.bfn0_filter_enable   = config->bfn0_filter_enable;
    portmod_config.bfn1_filter_enable   = config->bfn1_filter_enable;


    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_config_set(unit, port, index, &portmod_config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_gcw_filter_set(
    int unit,
    int port,
    uint8 index,
    bcm_cpri_tx_gcw_tx_filter_info_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_gcw_filter_set(unit, port, config->bfn0_value,config->bfn0_mask, config->bfn1_value, config->bfn1_mask));
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rsvd4_rx_tag_gen_entry_delete(/* PAPI delete */
    int unit, 
    int port, 
    int index, 
    bcm_cpri_tag_gen_entry_t *config)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_tx_cw_vsd_ctrl_flow_config_set(
    int unit, 
    int port, 
    uint16 group_id, 
    bcm_cpri_tx_vsd_flow_info_t *config) 
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_cpri_port_tx_vsd_flow_add(unit, port, group_id, config));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_port_rx_tag_config_set(
    int unit, 
    int port, 
    bcm_cpri_control_tag_config_t *config)
{
#ifdef CPRIMOD_SUPPORT
    BCM_IF_ERROR_RETURN
        (portmod_rx_tag_config_set(unit, port, config->default_tag, config->no_match_tag));

    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}

int bcm_esw_cpri_rx_tag_config_set( /* PAPI remove */
    int unit, 
    int port, 
    uint32 default_tag, 
    uint32 no_match_tag)
{
#ifdef CPRIMOD_SUPPORT
    return (BCM_E_NONE);
#else
    return (BCM_E_UNAVAIL);
#endif
}
