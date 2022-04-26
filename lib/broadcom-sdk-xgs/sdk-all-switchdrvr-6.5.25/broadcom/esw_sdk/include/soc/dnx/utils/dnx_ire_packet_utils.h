/*
 * $Id: dnx_ire_packet_utils.h
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef DNX_IRE_PACKET_UTILS_H_INCLUDED

#define DNX_IRE_PACKET_UTILS_H_INCLUDED

#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#define DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_BYTES             64
#define DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_WORDS             BYTES2WORDS(DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_BYTES)

/*
 * \brief - IRE Packet modes ENUM, describe the packet sending mode by IRE
 */
typedef enum
{
    DNX_IRE_PACKET_MODE_SINGLE_SHOT = 0,
    DNX_IRE_PACKET_MODE_CONST_RATE = 2
} dnx_ire_packet_mode_e;

/*
 * \brief - Information of packet to configure in IRE
 */
typedef struct
{
    /*
     * The sending mode of the packet
     */
    dnx_ire_packet_mode_e packet_mode;

    /*
     * The Packet rate in KHz
     * Invalid in single shot
     */
    int pakcet_rate_khz;

    /*
     * Number of Packets to send
     * Valid only in single shot
     */
    int nof_packets;

    /*
     * The Size of valid packet in bytes.
     * Limited to 64B (512 bits)
     */
    int valid_bytes;

    /*
     * Packet data, copy per core.
     * The first valid_bytes Bytes are used
     */
    uint32 packet_data_0[DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_WORDS];
    uint32 packet_data_1[DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_WORDS];

    /*
     * PTC value per core
     */
    int ptc_0;
    int ptc_1;

} dnx_ire_packet_control_info_t;

/*
 * \brief - Configure the IRE with the packet_info information
 *  Note, the packets are still not sent
 * \param [in] unit - Device Id
 * \param [in] packet_info - packet information to configure in IRE
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ire_packet_init(
    int unit,
    dnx_ire_packet_control_info_t * packet_info);

/*
 * \brief - Start to send packets according to IRE configuration
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ire_packet_start(
    int unit);

/*
 * \brief - Stop sending IRE packets
 * \param [in] unit - Device Id
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ire_packet_end(
    int unit);

#endif
