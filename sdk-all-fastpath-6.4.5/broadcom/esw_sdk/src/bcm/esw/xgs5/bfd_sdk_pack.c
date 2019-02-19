/*
 *
 * File:        bfd_sdk_pack.c
 * Purpose:     BFD unpack routines for:
 *              - BFD Control messages
 *
 */
#include <bcm_int/esw/bfd_sdk_pack.h>


uint8 *
bfd_sdk_version_exchange_msg_unpack(uint8 *buf, bfd_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bfd_sdk_version_exchange_msg_pack(uint8 *buf, bfd_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}

