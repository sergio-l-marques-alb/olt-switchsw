/** \file adapter_reg_access.h
 * This file handles the communication with the adapter's server
 */
/*
 * $Id:adapter_reg_access.h,v 1.312 Broadcom SDK $                                                           $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

#ifndef ADAPTER_REG_ACCESS_H_INCLUDED
/* { */
#define ADAPTER_REG_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifdef ADAPTER_SERVER_MODE

#include <soc/dnxc/dnxc_adapter_reg_access.h>

/**
 * \brief -  Client call: send packet to the adapter's server
 * unit - not in use
 * ms_id - ID of the block in the adapter
 * src_port - not in use
 * len - buf length in bytes
 * not_signals - number of signals in buf
 * Packet format: | length in bytes from ms_id(32 bits) | ms_id (32 bits) | nof_signals (32 bits) | SIGNALS (Signal id 32 bits | data length in bytes 32 bits | data )*
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] ms_id - module ID in the adapter
 *   \param [in] src_port - source port
 *   \param [in] src_channel - source channel
 *   \param [in] len - length of the buffer parameter
 *   \param [in] buf - buffer with the tx packet's data
 *   \param [in] nof_signals - number of signals
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   uint32
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32 adapter_send_buffer(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 src_port,
    uint32 src_channel,
    int len,
    unsigned char *buf,
    int nof_signals);

#endif
/**ADAPTER_REG_ACCESS_H*/
/* } */
#endif
