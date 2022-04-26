/** \file adapter_reg_access.c
 * This file presents an example of replacing the registers/memories access functions. It
 * assumes using the portmod register DB bcm2801pb_a0
 */
/*
 * $Id:$
 $Copyright: (c) 2021 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

int
tmp_workaround_func(
    void)
{
    return 0;
}
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX

#ifdef ADAPTER_SERVER_MODE
/*
 * {
 */

#include <netdb.h>
/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <bde/pli/verinet.h>
#include <errno.h>
#include <sys/socket.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnxc/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>

/*
 * This file should be taken out and all error
 * handling should be ported to DNX standard.
 */
#include <soc/dcmn/error.h>

#include <netinet/tcp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Maximum packet header size in the adapter. */
#define MAX_PACKET_HEADER_SIZE_ADAPTER 30

/** RX thread is about to be closed */
#define RX_THREAD_NOTIFY_CLOSED (-2)
/*
 * }
 */

/**
 * \brief - Reads a Tx buffer from the adapter's server.
 * Assumes that the buffer's length is a multiple of 4
 * Parameters: sockfd -socket file descriptor. Packet format:
 * length (4 bytes), block ID (4 bytes), NOF signals (4 bytes),
 * [signal ID (4 bytes), signal length (4 bytes), data]
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] sub_unit_id - sub unit id
 *   \param [in] ms_id - module ID in the adapter
 *   \param [in] nof_signals - number of signals
 *   \param [in] port_type - port type. (CPU/NIF/OLP/etc..)
 *   \param [in] port_channel - channel of the port
 *   \param [in] port_first_lane - port_first_lane that sent out the packet from sim to cpu, meaning adapter outgoing port.
 *                                 relevant if the ifc_type is NIF
  *   \param [in] len - length of the buffer parameter
 *   \param [in] buf - buffer of the rx packet
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
uint32
adapter_read_buffer(
    int unit,
    int sub_unit_id,
    adapter_ms_id_e * ms_id,
    uint32 *nof_signals,
    int *ifc_type,
    uint32 *port_channel,
    uint32 *port_first_lane,
    int *len,
    unsigned char *buf)
{
    fd_set read_vect;
    char swapped_header[MAX_PACKET_HEADER_SIZE_ADAPTER];
    uint32 packet_length;
    long long_val;
    int offset = 0;
    int nfds = 0;
    int rv = _SHR_E_NONE;
    /*
     * int out_port;
     */
    int adapter_rx_tx_fd;
    int constant_header_size;
    int adapter_ver;
    struct timeval tout;
    SHR_FUNC_INIT_VARS(unit);
    constant_header_size = dnx_data_adapter.rx.constant_header_size_get(unit);
    adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);
    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id, &adapter_rx_tx_fd);
    assert(adapter_rx_tx_fd);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("adapter_read_buffer: sockfd=%d\n"), adapter_rx_tx_fd));

    /** Setup bitmap for read notification*/
    FD_ZERO(&read_vect);

    /** Add Adapter Rx-Tx socket to selected fds*/
    FD_SET(adapter_rx_tx_fd, &read_vect);

    /** Add read end of pipe to selected fds*/
    FD_SET(pipe_fds[0], &read_vect);

    /** Set maximum fd */
    nfds = (adapter_rx_tx_fd > pipe_fds[0]) ? adapter_rx_tx_fd + 1 : pipe_fds[0] + 1;
    /*
     * Listen to two files:
     * - Adapter Rx-Tx socket (for incoming packets)
     * - Read end of pipe     (for thread exit notification)
     * Once a file contains information to be read, we read it and process
     * accordingly - handle packet or exit thread
     */
    tout.tv_sec = 0;
    tout.tv_usec = 10;
    rv = select(nfds, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, &tout);
    /** timeout - exit without printing error */
    if (rv == 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_TIMEOUT);
    }
    /** real error - exit with error */
    if ((rv < 0) && (errno != EINTR))
    {
        perror("get_command: select error");
        SHR_ERR_EXIT(_SHR_E_FAIL, "select function error");
    }

    /** Thread is about to be closed */
    if (FD_ISSET(pipe_fds[0], &read_vect))
    {
        return RX_THREAD_NOTIFY_CLOSED;
    }
    /** Data ready to be read.*/
    if (FD_ISSET(adapter_rx_tx_fd, &read_vect))
    {
        /** Read the length of the packet*/
        if (readn(adapter_rx_tx_fd, &(swapped_header[0]), constant_header_size) < constant_header_size)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter server disconnected\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_DISABLED);
            SHR_EXIT();
        }
        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        packet_length = long_val;
        offset += sizeof(uint32);

        /** source device id was added in version 4.0 */
        if (adapter_ver >= 4)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            /*
             * device_id_source = long_val;
             */
            offset += sizeof(uint32);
        }
        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        *ms_id = long_val;
        offset += sizeof(uint32);

        /** outgoing port type was added in version 5.0 */
        if (adapter_ver >= 5)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            *ifc_type = (int) long_val;
            offset += sizeof(uint32);
        }
        else
        {
            *ifc_type = -1;
        }

        /** out port was added in version 2.0 */
        if (adapter_ver >= 2)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            *port_first_lane = long_val;
            offset += sizeof(uint32);
        }

        /** channel was added on version 3.0 */
        if (adapter_ver >= 3)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            *port_channel = long_val;
            offset += sizeof(uint32);
        }

        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        *nof_signals = long_val;
        offset += sizeof(uint32);

        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("adapter_read_buffer: packet_length=%d ms_id=%d nof_signals=%d\n"), packet_length, *ms_id,
                  *nof_signals));
        /*
         * packet_length is equal to the packet's size minus the length field size (4 bytes)
         * The constant header was read already so the last part of the packet is left to be read.
         */
        *len = (packet_length + 4) - constant_header_size;
        /*
         * Read the Rx packet
         */
        if (readn(adapter_rx_tx_fd, buf, *len) < *len)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read packet\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** ADAPTER_SERVER_MODE */
#endif
