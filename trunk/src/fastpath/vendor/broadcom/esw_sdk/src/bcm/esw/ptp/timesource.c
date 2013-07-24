/*
 * $Id: timesource.c 1.1.2.25 Broadcom SDK $
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
 */
#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <sal/core/dpc.h>

#include <bcm/ptp.h>
#include <bcm_int/esw/ptp.h>
#include <bcm/error.h>

/* Constants and variables. */
#define PTP_TIMESOURCE_USE_MANAGEMENT_MSG                               (1)
#define PTP_TIMESOURCE_TODIN_MGMTMSG_FORMAT_LENGTH                     (32)
#define PTP_TIMESOURCE_TODIN_MGMTMSG_MASK_LENGTH (BCM_PTP_MAX_TOD_FORMAT_STRING)
#define PTP_TIMESOURCE_SIGNAL_OUTPUT_DATA_SIZE_OCTETS                  (17)


static const bcm_ptp_port_identity_t portid_all = 
    {{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, PTP_IEEE1588_ALL_PORTS};

static int _bcm_ptp_tod_out_enabled;

/*
 * Function:
 *      _bcm_esw_ptp_configure_serial_tod
 * Purpose:
 *      Configure TOD for serial output.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      ptp_id         - (IN) PTP stack ID.
 *      clock_num      - (IN) PTP clock number.
 *      uart_num       - (IN) UART number.
 *      offset         - (IN) ToD offset (nsec).
 *      delay          - (IN) ToD delay (nsec).
 *      fw_format_type - (IN) ToD format type selector (per firmware definition)
 *      format_str_len - (IN) ToD format string length.
 *      format_str     - (IN) ToD format string.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_esw_ptp_configure_serial_tod(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int uart_num,
    int64 offset,
    uint32 delay,
    int fw_format_type,
    int format_str_len,
    const char *format_str)
{
    int rv = BCM_E_UNAVAIL;

    bcm_ptp_port_identity_t portid;
    uint8 payload[PTP_MGMTMSG_PAYLOAD_CONFIGURE_TOD_OUT_SIZE_OCTETS] = {0};
    uint8 *curs = &payload[0];
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 
    int copy_len;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    /* Make payload. */
    sal_memcpy(curs, "BCM\0\0\0", 6);                            curs += 6;
    *curs++ = uart_num;
    _bcm_ptp_int64_write(curs, offset);     		curs += 8;
    _bcm_ptp_uint32_write(curs, delay);             curs += sizeof(uint32);
    *curs++ = fw_format_type;

    copy_len = format_str_len;
    if (copy_len > BCM_PTP_MAX_TOD_FORMAT_STRING) {
        copy_len = BCM_PTP_MAX_TOD_FORMAT_STRING;
    } else if (copy_len < 0) {
        copy_len = 0;
    }
    *curs++ = copy_len;

    if (format_str) {
        sal_memcpy(curs, format_str, copy_len);
    }
    curs += BCM_PTP_MAX_TOD_FORMAT_STRING;

    

    /* Increment cursor for TODOE L2 header. */
    curs += 18;

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        /* On error, target message to (all clocks, all ports) portIdentity. */
        portid = portid_all;
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid, 
             PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_CONFIGURE_TOD_OUT,
             payload, curs - payload, resp, &resp_len);

    return rv;
}

/*
 * Function:
 *      bcm_esw_ptp_tod_input_sources_set
 * Purpose:
 *      Set PTP TOD input(s).
 * Parameters:
 *      unit            - (IN) Unit number.
 *      ptp_id          - (IN) PTP stack ID.
 *      clock_num       - (IN) PTP clock number.
 *      num_tod_sources - (IN) Number of ToD inputs.
 *      tod_sources     - (IN) PTP ToD input configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_tod_input_sources_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_tod_sources,
    bcm_ptp_tod_input_t *tod_sources)
{
    int rv = BCM_E_UNAVAIL;

    bcm_ptp_port_identity_t portid;
    uint8 payload[PTP_MGMTMSG_PAYLOAD_CONFIGURE_TOD_IN_SIZE_OCTETS*2] = {0};
    uint8 *curs = &payload[0];
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    int64 offset;
    int fw_format_type = 0;
    uint8 *format_str = 0;
    uint8 *mask_str = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;   
    }

    COMPILER_64_SET(offset, 0, tod_sources[0].tod_offset_ns);
    

    switch (num_tod_sources) {
    case 0:
        /* already have values set to have no TOD in */
        break;
    case 1:
        fw_format_type = tod_sources[0].format + 1;  /* ordering must be same on fw side */
        format_str = tod_sources[0].format_str;
        mask_str = tod_sources[0].mask_str;
        break;
    default:
        /* only currently support a single TOD-in */
        return BCM_E_PARAM;
    }

    /* Make payload. */
    sal_memcpy(curs, "BCM\0\0\0", 6);                            curs += 6;
    *curs++ = tod_sources[0].source;
    _bcm_ptp_int64_write(curs, offset);     curs += 8;
    *curs++ = fw_format_type;

    *curs++ = (format_str) ? strlen((char*)format_str) : 0;

    if (format_str) {
        sal_memcpy(curs, format_str, PTP_TIMESOURCE_TODIN_MGMTMSG_FORMAT_LENGTH);
    }
    curs += PTP_TIMESOURCE_TODIN_MGMTMSG_FORMAT_LENGTH;

    *curs++ = (mask_str) ? strlen((char*)mask_str) : 0;

    if (mask_str) {
        sal_memcpy(curs, mask_str, PTP_TIMESOURCE_TODIN_MGMTMSG_MASK_LENGTH);
    }
    curs += PTP_TIMESOURCE_TODIN_MGMTMSG_MASK_LENGTH;

    if (tod_sources[0].peer_address.raw_l2_header_length == 18) {
        sal_memcpy(curs, tod_sources[0].peer_address.raw_l2_header + 6, 6); /* pick out src MAC */
        curs += 6;
        sal_memcpy(curs, tod_sources[0].peer_address.raw_l2_header + 14, 2); /* pick out VLAN */
        curs += 2;
        sal_memcpy(curs, tod_sources[0].peer_address.raw_l2_header + 16, 2); /* pick out EtherType */
        curs += 2;
    } else {
        curs += 6 + 2 + 2;
    }

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        /* On error, target message to (all clocks, all ports) portIdentity. */
        portid = portid_all;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_CONFIGURE_TOD_IN,
            payload, curs - payload, resp, &resp_len))) {
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_tod_input_set(unit, ptp_id, clock_num,
            num_tod_sources - 1, tod_sources))) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_ptp_tod_input_sources_get
 * Purpose:
 *      Get PTP ToD input source(s).
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      ptp_id          - (IN)  PTP stack ID.
 *      clock_num       - (IN)  PTP clock number.
 *      num_tod_sources - (IN)  Max number of ToD inputs.
 *      tod_sources     - (OUT) Array of ToD inputs.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_tod_input_sources_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_tod_sources,
    bcm_ptp_tod_input_t *tod_sources)
{
    int rv = BCM_E_UNAVAIL;
    int i;

    bcm_ptp_port_identity_t portid;
    uint8 resp[PTP_MGMTMSG_PAYLOAD_CONFIGURE_TOD_IN_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_CONFIGURE_TOD_IN_SIZE_OCTETS;

    uint8 fmtlen = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    
    if (num_tod_sources >= PTP_MAX_TOD_INPUTS) {
        num_tod_sources = PTP_MAX_TOD_INPUTS;
    }

    if (PTP_TIMESOURCE_USE_MANAGEMENT_MSG) {
        if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
                clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
                &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_CONFIGURE_TOD_IN, 0, 0,
                resp, &resp_len))) {
            return rv;
        }

        /*
         * Parse response.
         *    Octet 0...5    : Custom management message key/identifier. 
         *                     BCM<null><null><null>.
         *    Octet 6        : UART number.    
         *    Octet 7...14   : ToD-In offset (nsec).
         *    Octet 15       : ToD-In format.
         *    Octet 16       : ToD-In format string length.
         *    Octet 17...48  : ToD-In format string.
         *    Octet 49       : ToD-In mask string length.
         *    Octet 50...177 : ToD-In mask string.
         */
        tod_sources[0].source = bcmPTPTODSourceSerial; 

        i = 7;    
        {
            int32 offset_hi = _bcm_ptp_uint32_read(resp+i);
            int32 offset_lo = _bcm_ptp_uint32_read(resp+i+4);
            int64 offset;
            COMPILER_64_SET(offset, offset_hi, offset_lo);
            tod_sources[0].tod_offset_ns = offset_lo;  
        }
        i += sizeof(uint64);

        /*
         * ToD format enumeration.
         * Account for mapping to firmware definition.
         */
        tod_sources[0].format = resp[i++] - 1; 

        fmtlen = resp[i++];
        sal_memset(tod_sources[0].format_str, 0, BCM_PTP_MAX_TOD_FORMAT_STRING);
        sal_memcpy(tod_sources[0].format_str, resp + i, fmtlen);
        i += PTP_TIMESOURCE_TODIN_MGMTMSG_FORMAT_LENGTH;

        fmtlen = resp[i++];
        sal_memset(tod_sources[0].mask_str, 0, BCM_PTP_MAX_TOD_FORMAT_STRING);
        sal_memcpy(tod_sources[0].mask_str, resp + i, fmtlen);

    } else {
        for (i = 0; i < num_tod_sources; ++i) {
            if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_tod_input_get(unit, ptp_id, 
                    clock_num, i, &tod_sources[i]))) {
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_ptp_timesource_input_status_get
 * Purpose:
 *      Get time source status.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      ptp_id      - (IN)  PTP stack ID.
 *      clock_num   - (IN)  PTP clock number.
 *      status      - (OUT) Time source status.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int 
bcm_esw_ptp_timesource_input_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timesource_status_t *status)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_esw_ptp_input_channels_set
 * Purpose:
 *      Set PTP input channels.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      ptp_id       - (IN) PTP stack ID.
 *      clock_num    - (IN) PTP clock number.
 *      num_channels - (IN) Number of channels (time sources).
 *      channels     - (IN) Channels (time sources).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_input_channels_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_channels,
    bcm_ptp_channel_t *channels)
{
    int rv = BCM_E_UNAVAIL;
    int i;

    bcm_ptp_port_identity_t portid;
    uint8 payload[PTP_MGMTMSG_PAYLOAD_CHANNELS_SIZE_OCTETS] = {0};
    uint8 *curs = &payload[0];
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id,
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        return rv;
    }

    if (num_channels >= PTP_MAX_TIME_SOURCES) {
        num_channels = PTP_MAX_TIME_SOURCES;
    } else if (num_channels < 1) {
        num_channels = 1;
    }
    
    /* Make payload. */
    _bcm_ptp_uint16_write(curs, num_channels);  curs += sizeof(uint16);

    for (i = 0; i < num_channels; ++i) {
        _bcm_ptp_uint16_write(curs, channels[i].type);         curs += sizeof(uint16);
        _bcm_ptp_uint32_write(curs, channels[i].source);       curs += sizeof(uint32);
        _bcm_ptp_uint32_write(curs, channels[i].frequency);    curs += sizeof(uint32);

        *curs++ = channels[i].tod_index;

        *curs++ = channels[i].freq_priority;
        *curs++ = channels[i].freq_enabled;
        *curs++ = channels[i].time_prio;
        *curs++ = channels[i].time_enabled;

        *curs++ = channels[i].freq_assumed_QL;
        *curs++ = channels[i].time_assumed_QL;
        *curs++ = channels[i].assumed_QL_enabled;

        _bcm_ptp_uint32_write(curs, channels[i].resolution);   curs += sizeof(uint32);
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid,
             PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_CHANNELS,
             payload, curs - payload, resp, &resp_len);

    return rv; 
}

/*
 * Function:
 *      _bcm_esw_ptp_input_channels_get
 * Purpose:
 *      Set PTP input channels.
 * Parameters:
 *      unit         - (IN)  Unit number.
 *      ptp_id       - (IN)  PTP stack ID.
 *      clock_num    - (IN)  PTP clock number.
 *      num_channels - (IN/  Max number of channels (time sources) / 
 *                      OUT) Number of returned channels (time sources).
 *      channels     - (OUT) Channels (time sources).
 * Returns:
 *      BCM_E_XXX - Function status;
 * Notes:  
 *      Function is not part of external API at this point.
 */
int
_bcm_esw_ptp_input_channels_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_channels,
    bcm_ptp_channel_t *channels)
{
    int rv = BCM_E_UNAVAIL;
    int i;

    bcm_ptp_port_identity_t portid;
    int max_num_channels = *num_channels;

    uint8 resp[PTP_MGMTMSG_PAYLOAD_CHANNELS_SIZE_OCTETS] = {0};
    uint8 *curs = &resp[0];
    int resp_len = PTP_MGMTMSG_PAYLOAD_CHANNELS_SIZE_OCTETS;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        return rv;
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid,
             PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_CHANNELS, 0, 0, resp, &resp_len);

    /* Parse response. */
    *num_channels = _bcm_ptp_uint16_read(curs);  curs += sizeof(uint16);

    if (*num_channels > max_num_channels) {
        *num_channels = max_num_channels;
    }

    for (i = 0; i < *num_channels; ++i) {
        channels[i].type = _bcm_ptp_uint16_read(curs);         curs += sizeof(uint16);
        channels[i].source = _bcm_ptp_uint32_read(curs);       curs += sizeof(uint32);
        channels[i].frequency = _bcm_ptp_uint32_read(curs);    curs += sizeof(uint32);
        channels[i].tod_index = *curs++;
        channels[i].freq_priority = *curs++;
        channels[i].freq_enabled  = *curs++;
        channels[i].time_prio = *curs++;
        channels[i].time_enabled = *curs++;
        channels[i].freq_assumed_QL = *curs++;
        channels[i].time_assumed_QL = *curs++;
        channels[i].assumed_QL_enabled = *curs++;
        channels[i].resolution = _bcm_ptp_uint32_read(curs);   curs += sizeof(uint32);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_ptp_input_channels_status_get
 * Purpose:
 *      Get status of channels
 * Parameters:
 *      unit        - (IN)     Unit number.
 *      ptp_id      - (IN)     PTP stack ID.
 *      clock_num   - (IN)     PTP clock number.
 *      num_chan    - (IN/OUT) Number of channels to return / returned
 *      chan_status - (OUT)    Array of channel statuses
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_esw_ptp_input_channels_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_chan,
    _bcm_ptp_channel_status_t *chan_status)
{
    int rv = BCM_E_UNAVAIL;
    int i;
    int num_returned_channels = 0;

    uint8 resp[PTP_MGMTMSG_PAYLOAD_CHANNEL_STATUS_SIZE_OCTETS] = {0};
    uint8 *curs = &resp[6];
    int resp_len = PTP_MGMTMSG_PAYLOAD_CHANNEL_STATUS_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (*num_chan < 1) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id,
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        return rv;
    }

    if (BCM_FAILURE (rv = _bcm_ptp_management_message_send(unit, ptp_id,
            clock_num, &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_CHANNEL_STATUS,
            0, 0, resp, &resp_len))) {
        return rv;
    }

    /* Parse response. */
    num_returned_channels = _bcm_ptp_uint16_read(curs);  curs += sizeof(uint16);

    if (num_returned_channels < *num_chan) {
        *num_chan = num_returned_channels;
    }

    for (i = 0; i < *num_chan; ++i) {
        chan_status[i].freq_status = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].time_status = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].freq_weight = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].time_weight = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].freq_QL  = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].time_QL = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].ql_read_externally = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
        chan_status[i].fault_map = _bcm_ptp_uint32_read(curs);
        curs += sizeof(uint32);
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      bcm_esw_ptp_input_channel_precedence_mode_set
 * Purpose:
 *      Set PTP input channels precedence mode.
 * Parameters:
 *      unit                - (IN) Unit number.
 *      ptp_id              - (IN) PTP stack ID.
 *      clock_num           - (IN) PTP clock number.
 *      channel_select_mode - (IN) Input channel precedence mode selector.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_input_channel_precedence_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_select_mode)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_esw_ptp_input_channel_switching_mode_set
 * Purpose:
 *      Set PTP input channels switching mode.
 * Parameters:
 *      unit                   - (IN) Unit number.
 *      ptp_id                 - (IN) PTP stack ID.
 *      clock_num              - (IN) PTP clock number.
 *      channel_switching_mode - (IN) Channel switching mode selector.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_input_channel_switching_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_switching_mode)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_esw_ptp_tod_output_set
 * Purpose:
 *      Set PTP ToD output.
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      ptp_id        - (IN)  PTP stack ID.
 *      clock_num     - (IN)  PTP clock number.
 *      tod_output_id - (OUT) ToD output ID.
 *      output_info   - (IN)  ToD output configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_tod_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_id,
    bcm_ptp_tod_output_t *output_info)
{
    int rv = BCM_E_UNAVAIL;
    int fw_format_type = output_info->format + 1;  /* relies on firmware format type having the same ordering */
    int64 offset;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    COMPILER_64_SET(offset, 0, output_info->tod_offset_ns);
    if (BCM_FAILURE(rv = _bcm_esw_ptp_configure_serial_tod(unit, ptp_id, clock_num,
            0, offset, output_info->tod_delay_ns,
            fw_format_type, output_info->format_str_len, (char *)output_info->format_str))) {
        return rv;
    }

    *tod_output_id = 1;
    _bcm_ptp_tod_out_enabled = 1;

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_tod_output_set(unit, ptp_id, clock_num,
            *tod_output_id - 1, output_info))) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_ptp_tod_output_get
 * Purpose:
 *      Get PTP TOD output(s).
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      ptp_id           - (IN)  PTP stack ID.
 *      clock_num        - (IN)  PTP clock number.
 *      tod_output_count - (IN/  Max number of ToD outputs / 
 *                          OUT) Number ToD outputs returned.
 *      tod_output_id    - (OUT) Array of ToD outputs.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_tod_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_count,
    bcm_ptp_tod_output_t *tod_output_id)
{
    int rv = BCM_E_UNAVAIL;
    int i;

    bcm_ptp_port_identity_t portid;
    uint8 resp[PTP_MGMTMSG_PAYLOAD_CONFIGURE_TOD_OUT_SIZE_OCTETS] = {0};
    int resp_len = PTP_MGMTMSG_PAYLOAD_CONFIGURE_TOD_OUT_SIZE_OCTETS;

    

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (*tod_output_count >= PTP_MAX_TOD_OUTPUTS) {
        *tod_output_count = PTP_MAX_TOD_OUTPUTS;
    }

    if (PTP_TIMESOURCE_USE_MANAGEMENT_MSG) {
        if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
                clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
                &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_CONFIGURE_TOD_OUT, 0, 0,
                resp, &resp_len))) {
            return rv;
        }

        /*
         * Parse response.
         *    Octet 0...5    : Custom management message key/identifier. 
         *                     BCM<null><null><null>.
         *    Octet 6        : UART number.    
         *    Octet 7...14   : ToD-Out offset (nsec).
         *    Octet 15...18  : ToD-Out delay (nsec).
         *    Octet 19       : ToD-Out format.
         *    Octet 20       : ToD-Out format string length.
         *    Octet 21...148 : ToD-Out format string.
         */
        tod_output_id[0].source = bcmPTPTODSourceSerial; 
        tod_output_id[0].frequency = 1;                  

        i = 7;    
        {
            int32 offset_hi = _bcm_ptp_uint32_read(resp+i);
            int32 offset_lo = _bcm_ptp_uint32_read(resp+i+4);
            int64 offset;
            COMPILER_64_SET(offset, offset_hi, offset_lo);
            tod_output_id[0].tod_offset_ns = offset_lo;  
        }
        i += sizeof(uint64);

        tod_output_id[0].tod_delay_ns = _bcm_ptp_uint32_read(resp+i);
        i += sizeof(uint32);
        
        /*
         * ToD format enumeration.
         * Account for mapping to firmware definition.
         */
        tod_output_id[0].format = resp[i++] - 1;
        
        if (tod_output_id[0].format < bcmPTPTODFormatString) {
            tod_output_id[0].format = bcmPTPTODFormatString;
        }

        tod_output_id[0].format_str_len = resp[i++];
        sal_memset(tod_output_id[0].format_str, 0, BCM_PTP_MAX_TOD_FORMAT_STRING);
        sal_memcpy(tod_output_id[0].format_str, resp + i, tod_output_id[0].format_str_len);

    } else {
        for (i = 0; i < *tod_output_count; ++i) {
            if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_tod_output_get(unit, ptp_id, 
                    clock_num, i, &tod_output_id[i]))) {
                return rv;
            }
        }
    }

    return BCM_E_NONE; 
}

/*
 * Function:
 *      bcm_esw_ptp_tod_output_remove
 * Purpose:
 *      Remove a ToD output.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      ptp_id        - (IN) PTP stack ID.
 *      clock_num     - (IN) PTP clock number.
 *      tod_output_id - (IN) ToD output ID.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_tod_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_output_id)
{
    int rv = BCM_E_UNAVAIL;
    int fw_format_type = 0;
    int64 offset;

    if (!_bcm_ptp_tod_out_enabled) {
        return BCM_E_PARAM;
    }

    COMPILER_64_SET(offset, 0, 0); 
    if (BCM_FAILURE(rv = _bcm_esw_ptp_configure_serial_tod(unit, ptp_id, clock_num,
            0, offset, 0, fw_format_type, 0, 0))) {
        return rv;
    }

    _bcm_ptp_tod_out_enabled = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_ptp_signal_output_set
 * Purpose:
 *      Set PTP signal output.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      ptp_id           - (IN)  PTP stack ID.
 *      clock_num        - (IN)  PTP clock number.
 *      signal_output_id - (OUT) ID of signal.
 *      output_info      - (IN)  Signal information.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_signal_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_id,
    bcm_ptp_signal_output_t *output)
{
    int rv = BCM_E_UNAVAIL;
    unsigned idx;

    bcm_ptp_signal_output_t signal;
    bcm_ptp_port_identity_t portid;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_OUTPUT_SIGNALS_SIZE_OCTETS] = {0};
    uint8 *curs = &payload[0];
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    idx = output->pin;
    if (idx >= PTP_MAX_OUTPUT_SIGNALS) {
        return BCM_E_PARAM;
    }

    /* use the pin # as the signal_output_id, since they're unique anyhow  */
    /* this makes the "sparse" problem immediately apparent if the pin# is */
    /* not zero, but the problem would occur anyhow if any but the first   */
    /* signal were removed                                                 */

    *signal_output_id = output->pin;

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_signal_set(unit, ptp_id, clock_num,
            idx, output))) {
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        return rv;
    }

    /* Make payload. */
    if (PTP_TIMESOURCE_USE_MANAGEMENT_MSG) {
        /* Get PTP signal output configurations from firmware. */
        if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id,
             clock_num, &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_OUTPUT_SIGNALS,
             0, 0, resp, &resp_len))) {
            return rv;
        }
        /* Insert updated configuration for chosen PTP signal output. */
        sal_memcpy(payload, resp, PTP_MGMTMSG_PAYLOAD_OUTPUT_SIGNALS_SIZE_OCTETS);
        /* Advance cursor. */
        curs += (output->pin*PTP_TIMESOURCE_SIGNAL_OUTPUT_DATA_SIZE_OCTETS);
        _bcm_ptp_uint32_write(curs, output->pin);
        curs += sizeof(uint32);
        _bcm_ptp_uint32_write(curs, output->frequency);
        curs += sizeof(uint32);
        *curs++ = output->phase_lock;
        _bcm_ptp_uint32_write(curs, output->pulse_width_ns);
        curs += sizeof(uint32);
        _bcm_ptp_uint32_write(curs, output->pulse_offset_ns);
    } else {
        /* Get PTP signal output configurations from SDK cache. */
        for (idx = 0; idx < PTP_MAX_OUTPUT_SIGNALS; ++idx) {
            if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_signal_get(unit, ptp_id,
                    clock_num, idx, &signal))) {
                return rv;
            }
            _bcm_ptp_uint32_write(curs, signal.pin);               curs += sizeof(uint32);
            _bcm_ptp_uint32_write(curs, signal.frequency);         curs += sizeof(uint32);
            *curs++ = signal.phase_lock;
            _bcm_ptp_uint32_write(curs, signal.pulse_width_ns);    curs += sizeof(uint32);
            _bcm_ptp_uint32_write(curs, signal.pulse_offset_ns);   curs += sizeof(uint32);
        }
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid, 
             PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_OUTPUT_SIGNALS,
             payload, PTP_MGMTMSG_PAYLOAD_OUTPUT_SIGNALS_SIZE_OCTETS,
             resp, &resp_len);

    return rv;
}

/*
 * Function:
 *      bcm_esw_ptp_signal_output_get
 * Purpose:
 *      Get PTP signal output.
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      ptp_id              - (IN)  PTP stack ID.
 *      clock_num           - (IN)  PTP clock number.
 *      signal_output_count - (IN/  Max number of signal outputs / 
 *                             OUT) Number signal outputs returned.
 *      signal_output       - (OUT) Array of signal outputs.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes: 
 *      This function specification does not provide the ID of each signal,
 *      so the signal ID equals the position in the array, i.e. there is no
 *      mechanism to make it sparse. Invalid/inactive outputs are indicated
 *      by a zero (0) frequency.
 */
int
bcm_esw_ptp_signal_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_count,
    bcm_ptp_signal_output_t *signal_output)
{
    int rv = BCM_E_UNAVAIL;
    int i;

    bcm_ptp_port_identity_t portid;
    uint8 resp[PTP_MGMTMSG_PAYLOAD_OUTPUT_SIGNALS_SIZE_OCTETS] = {0};
    uint8 *cursor = &resp[0];
    int resp_len = PTP_MGMTMSG_PAYLOAD_OUTPUT_SIGNALS_SIZE_OCTETS;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    *signal_output_count = 0;

    if (PTP_TIMESOURCE_USE_MANAGEMENT_MSG) {
        if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id,
                clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
                &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_OUTPUT_SIGNALS,
                0, 0, resp, &resp_len))) {
            return rv;
        }

        /*
         * Parse response.
         *    Per Output Signal : # = Output Signal Number, I = 17(#-1)
         *    Octet I...I+3     : Output Signal # pin.
         *    Octet I+4...I+7   : Output Signal # frequency.
         *    Octet I+8         : Output Signal # phase-lock Boolean.
         *    Octet I+9...I+12  : Output Signal # pulse width (nsec).
         *    Octet I+13...I+16 : Output Signal # pulse offset (nsec).
         */
        for (i = 0; i < PTP_MAX_OUTPUT_SIGNALS; ++i) {
            if (cursor >= (resp_len + &resp[0])) 
            {
                return BCM_E_NONE;
            }
            signal_output[i].pin = _bcm_ptp_uint32_read(cursor);
            cursor += sizeof(uint32);
            signal_output[i].frequency = _bcm_ptp_uint32_read(cursor);
            cursor += sizeof(uint32);

            signal_output[i].phase_lock = *cursor++;

            signal_output[i].pulse_width_ns = _bcm_ptp_uint32_read(cursor);
            cursor += sizeof(uint32);
            signal_output[i].pulse_offset_ns = _bcm_ptp_uint32_read(cursor);
            cursor += sizeof(uint32);
            if (signal_output[i].frequency)
            {
                *signal_output_count = (*signal_output_count) + 1;
            }
        }

    } else {
        for (i = 0; i < *signal_output_count; ++i) {
            if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_signal_get(unit, ptp_id,
                    clock_num, i, signal_output))) {
                return rv;
            }

            ++signal_output;
        }
    }

    return BCM_E_NONE;    
}

/*
 * Function:
 *      bcm_esw_ptp_signal_output_remove
 * Purpose:
 *      Remove PTP signal output.
 * Parameters:
 *      unit             - (IN) Unit number.
 *      ptp_id           - (IN) PTP stack ID.
 *      clock_num        - (IN) PTP clock number.
 *      signal_output_id - (IN) Signal to remove/invalidate.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
bcm_esw_ptp_signal_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int signal_output_id)
{
    if (SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
        bcm_ptp_signal_output_t signal_output = {0};
        signal_output.pin = signal_output_id;

        return bcm_esw_ptp_signal_output_set(unit, ptp_id, clock_num, &signal_output_id, &signal_output);
    }

    return BCM_E_UNAVAIL;
}

#endif /* defined(INCLUDE_PTP)*/
