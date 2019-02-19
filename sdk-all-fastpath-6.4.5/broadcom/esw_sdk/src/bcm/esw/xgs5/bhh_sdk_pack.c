/*
 *
 * File:        bhh_sdk_pack.c
 * Purpose:     BHH pack and unpack routines for:
 *              - BHH Control messages
 *
 */
#include <bcm_int/esw/bhh_sdk_pack.h>
#include <shared/pack.h>

uint8 *
bhh_sdk_version_exchange_msg_unpack(uint8 *buf, bhh_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bhh_sdk_version_exchange_msg_pack(uint8 *buf, bhh_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 bhh_sdk_msg_ctrl_sess_get_t *msg)
{ 
    int i;

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive);
    _SHR_UNPACK_U8(buf, msg->local_demand);
    _SHR_UNPACK_U8(buf, msg->remote_demand);
    _SHR_UNPACK_U8(buf, msg->local_sess_state);
    _SHR_UNPACK_U8(buf, msg->remote_sess_state);
    _SHR_UNPACK_U8(buf, msg->mel);
    _SHR_UNPACK_U16(buf, msg->mep_id);
    for (i = 0; i < SHR_BHH_MEG_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->meg_id[i]);
    }
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U8(buf, msg->encap_type);
    _SHR_UNPACK_U32(buf, msg->encap_length);
    for (i = 0; i < SHR_BHH_MAX_ENCAP_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_UNPACK_U32(buf, msg->tx_port);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U32(buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    if(BHH_UC_FEATURE_CHECK(BHH_WB_SESS_GET)) {
        _SHR_UNPACK_U32(buf, msg->mpls_label);
        _SHR_UNPACK_U32(buf, msg->if_num);
        _SHR_UNPACK_U32(buf, msg->flags);
    }
    return buf;
}

